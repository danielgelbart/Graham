#include <iostream>
#include <fstream>
#include <boost/filesystem/fstream.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "Parser.h"
#include "Url.h"
#include "Logger.h"
#include "types.h"
#include "dmmm_dbface.h"
#include "T_Stock.hpp"
#include "T_Ep.hpp"
#include "Financials.h"

using namespace std;
using namespace DMMM;

void
EdgarData::downloadAndSave10k(Url& url, Info& info)
{
    const path& writeDest = FINANCIALS_PATH / info.ticker;
    cout << "\n  Path to store file is: " << writeDest << endl;
    downloadAndSave(url,info,writeDest);
}

bool
EdgarData::downloadToString(Url& url, string& rContent)
{
    return mHttpClient.httpGet(url.fullAddr(), "text/html", rContent);
}

void
EdgarData::downloadAndSave(Url& url, Info& info, const path& writeDest)
{
    string rContent; // will hold the returned text
    mHttpClient.httpGet(url.fullAddr(), "text/html", rContent);

    string infoString( info.ticker + "_" + to_string(info.year) );
    write_to_disk(rContent, infoString, writeDest);
}

string
EdgarData::getEdgarSearchResultsPage(O_Stock& stock, StatementType st)
{
    string cik( to_string(stock._cik()) );
    string statType = "10-k";
    if ( st == StatementType::Q10 )
        statType = "10-q";

    string uri = "http://www.sec.gov/cgi-bin/browse-edgar?action=getcompany&CIK=" + cik + "&type=" + statType + "&dateb=&owner=exclude&count=40";

    Url url = Url(uri);
    string page;
    downloadToString( url, page);
    return page;
}

string
EdgarData::getEdgarFiling( O_Stock& stock, Acn& acn)
{
    string cik( to_string(stock._cik()) );
    string uri("http://www.sec.gov/Archives/edgar/data/");
    uri += cik + "/" + acn._acn + ".txt";
    Url url = Url(uri);
    string page;
    
    // check for 404
    if (! downloadToString( url, page ) )
    {
        LOG_ERROR << "\n failed to retrieve webpage." <<
            " NOT getting info for quarter " << to_string(acn._quarter) << "\n";
        return "";
    }
    return page;
}

Acn*
EdgarData::getLastYear10KAcn( O_Stock& stock)
{
    string page = getEdgarSearchResultsPage( stock , StatementType::K10);
    Parser parser;
    vector<Acn*> Acns = parser.getAcnsFromSearchResults( page, 1,/*limit*/ 
                                                         StatementType::K10 );
    return Acns.front();
}

bool
stock_contains_acn(O_Stock& stock, Acn& acn)
{
    vector<O_Ep> epss = stock._eps();
    for (auto it = epss.begin() ; it != epss.end(); ++it )
        if ( it->_year()    == acn._report_date.year() &&
             it->_quarter() == acn._quarter )
            return true;
    return false;
}

bool
EdgarData::getQuarters(O_Stock& stock)
{
    // get back to Q1 LAST year
    string page = getEdgarSearchResultsPage(stock,StatementType::Q10);
    Parser parser;
    vector<Acn*> qAcns = parser.getAcnsFromSearchResults( page, 7,/*limit*/ 
                                                          StatementType::Q10 );
    bool updated(false);
    for(auto it = qAcns.begin() ; it != qAcns.end(); ++it)
    {
        cout << "\n Going to get Acn: " << (*it)->_acn << " date: " << 
            (*it)->_report_date << " quartr: " << (*it)->_quarter << endl;
        
        if ( ! stock_contains_acn( stock, *(*it) ) )
        {
            cout << "\n Stock does not have this data yet" << endl;
            string page = getEdgarFiling( stock, *(*it));
            addQuarterIncomeStatmentToDB( *(*it), stock, page );
            updated = true;
        }
    }  
    return updated;
}    

void
EdgarData::addQuarterIncomeStatmentToDB(Acn& acn, O_Stock& stock, 
                                        string& q10filingStr)
{
    //string page = getEdgarFiling( stock, acn);
    Parser parser = Parser();
    string incomeStr = parser.extract_quarterly_income(q10filingStr);

    incomeStr = parser.extractFirstTableStr( incomeStr );
    XmlElement* tree = parser.buildXmlTree(incomeStr);

    string units;
    string currency;
    string revenue;
    string income;
    float eps;
    string numshares;
    parser.parseQuarterlyIncomeStatment(tree, units, currency,
                                        revenue, income, eps, numshares);
    
    addEearningsRecordToDB( stock, acn._report_date.year(), acn._quarter,
                            revenue, income, eps,
                            numshares, "edgar.com"/*source*/);
}

bool
EdgarData::addEearningsRecordToDB( O_Stock& stock, size_t year, size_t quarter,
                                   string revenue, string income, float eps,
                                   string shares, const string& source)
{
    LOG_INFO << "\nGoing to try to insert earnings record to DB for "
             << stock._ticker() << " for year " << to_string(year) 
             << " quarter " << to_string(quarter);

    bool inserted(false);
    O_Ep incomeS( stock._id() );
    incomeS._year() = year;
    incomeS._quarter() = quarter;    
    incomeS._revenue() = revenue;
    incomeS._net_income() = income;
    incomeS._eps() = eps;
    incomeS._shares() = shares;
    incomeS._source() = source;
        
    if ( (inserted = insertEp(incomeS)) )
    {
        stringstream message;
        message << "\nEntered NEW Earnings record to DB for " << stock._ticker()
                <<". YEAR: "<< incomeS._year() << " Q: "<< incomeS._quarter()
                <<" REV: "<<incomeS._revenue()<<" INC: "<<incomeS._net_income()
                <<" SHARES: "<<incomeS._shares()<<" EPS: "<<incomeS._eps();
        LOG_INFO << "\n"<< message.str() <<"\n";
        cout << message.str() << endl;
    } else {
        LOG_ERROR << "\n FAILED to insert to DB earnings for year" 
                  << to_string(year) << ". ";
    }
    return inserted;
}

bool
EdgarData::insertEp( O_Ep& ep )
{
    // check that ep is valid
    if ( (ep._stock_id() <= 0 )         || 
         (ep._year() < greg_year(1980)) ||
         (ep._year() > greg_year(2020)) ||
         (ep._quarter() > 5 )           ||
         (ep._quarter() < 0 )           ||
         (ep._revenue() == "")          )
        return false;

    T_Stock ta;
    I_Stock stock_id( ep._stock_id() );
    pair<O_Stock, bool> spair = ta.select( stock_id );
    if (!spair.second)
        return false;
   
    O_Stock stock = spair.first;
   
    // check if it already exits in DB
    string dummy("dummy "); 
    date dd( ep._year(),Jan,1);
    Acn acn( dummy, dd, ep._quarter() );
    if ( stock_contains_acn( stock, acn ) )
        return false;

    // insert
    ep.insert();
    return true;
}

long
convert_string_to_long( string& str)
{
    long ret(1);
    size_t pos;
    if ( (pos = str.find("Bil")) != string::npos )
        return stod( str.substr(0,pos) ) * 1000000000 ;
    if ( (pos = str.find("Mil")) != string::npos )
        return stod( str.substr(0,pos) )* 1000000;
    return ret;
}

void
EdgarData::createFourthQuarter(O_Stock& stock, size_t year)
{
    T_Ep t;
    vector<O_Ep> incs = stock._eps( t._year() == year );

    if ( incs.size() != 4 )
    {
        LOG_ERROR << "Cannot create fourth quarter. Missing quarters ";
        return;
    }

    long revp(0);
    long incp(0);
    long numshares(1);
    for ( auto it = incs.begin(); it != incs.end(); ++it)
    {
        if ( it->_quarter() == 0 )
        {
            revp += stol( it->_revenue() );
            incp += stol( it->_net_income() );
            string annual_shares = it->_shares();
            if ( annual_shares.find("il") != string::npos )
                numshares = convert_string_to_long( annual_shares );
            else    
                numshares = stol( annual_shares );
            continue;
        }
        revp -= stol( it->_revenue() );
        incp -= stol( it->_net_income() );
    }
    addEearningsRecordToDB( stock, year, 4,/*quarter*/
                            to_string( revp ),/*revenue*/
                            to_string( incp ),/*income*/
                            ((double)incp) / numshares, /* EPS */
                            to_string(numshares),  
                            "calculated"/*source*/);
}

struct ep_comp {
    bool operator() (const O_Ep& i, const O_Ep& j) const 
        { 
            if ( i._year() != j._year() )
                return (i._year() > j._year() );
            return ( i._quarter() > j._quarter() );
        }
} ep_comp;

void
EdgarData::createTtmEps(O_Stock& stock)
{
    // delete last ttm
    T_Ep t;
//    O_Ep old_ttm = stock._eps( t._quarter() == 5)[0];
    string table("eps");
    string where = "stock_id=" + to_string(stock._id())+" AND quarter=5";
    
    DBFace::instance()->erase(table,where);

    // get all quarter reports, and SORT - NEWEST FIRST
    vector<O_Ep> qrts = stock._eps( t._quarter() > 0);
    if ( qrts.size() < 4 )
    {
        LOG_ERROR << " Stock does not have enough quarters."
             << " Cannot procede to calculate ttm eps";
        return;
        
    }
    sort( qrts.begin(), qrts.end(), ep_comp );

    // sum latest 4 quarters
    long revp(0);
    long incp(0);
    bool check[4] = {false,false,false,false};

    for( size_t i=0 ; i < 4; ++i)
    {
        revp += stol( qrts[i]._revenue() );
        incp += stol( qrts[i]._net_income() );
        check[ qrts[i]._quarter() - 1 ] = true;
    }
    if ( !(check[0] && check[1] && check[2] && check[3]) )
    {
        LOG_ERROR << " Mising a quarter for " << stock._ticker() <<
            " cannot procede to calculate ttm eps";
        return;
    }
    long numshares = stol( qrts[0]._shares() );
    
    addEearningsRecordToDB( stock, qrts[0]._year(), 5,/*quarter*/
                            to_string(revp), to_string(incp),
                            ((double)incp) / numshares, /*EPS*/
                            to_string(numshares),
                            "calculated");
}

bool 
EdgarData::getSingleYear(O_Stock& stock, size_t year)
{
    bool retVal(true);
 
   // get search page for 10k
    string page = getEdgarSearchResultsPage( stock, StatementType::K10);
    Parser parser;
 
// get All Acns
    // 1) Change name
    // 2) compare annual and quarter url strings to use same method
    vector<Acn*> Acns = parser.getAcnsFromSearchResults( page, 10,/*limit*/ 
                                                         StatementType::K10 );
    greg_year gyear(year);
    Acn* acn;
    for(auto it = Acns.begin() ; it != Acns.end(); ++it)
    {
        LOG_INFO << "\n Got Acn: " << (*it)->_acn << " date: " << 
            (*it)->_report_date << " quartr: " << (*it)->_quarter;
        
        if ( ! stock_contains_acn( stock, *(*it) ) )
        {
            if( ((*it)->_report_date.year() - 1) == gyear )
            {
                acn = *it;
                LOG_INFO << "Got ACN for specific year " << to_string(year);
                break;
            }
        }
    }  

// get Specific one for year to a string
    string filing = getEdgarFiling(stock,*acn);
// extract to DB
    Info info( stock._ticker(), gyear, StatementType::K10);
    extract10kToDisk( filing, stock, info);

    return retVal;
}

/*
Download the latest available 10-k for a specific ticker
save only the extracted financial statments to disk
*/
void 
EdgarData::updateFinancials(O_Stock& stock)
{
    LOG_INFO << "updateFinancials method was called for " << stock._ticker() << "\n";
    string cik = to_string( stock._cik() );
// get current date
    date today = day_clock::local_day();
    greg_year last_year = today.year() - 1;

    bool updated(false);
    updated = getQuarters( stock );

// check if last years 10k exists
    T_Ep t;

    if (stock._eps( t._year() == last_year &&  
                    t._quarter() == 0 ).empty() )
    {
        cout << "\n Going to get last year 10k" << endl;
        Acn* acn  = getLastYear10KAcn( stock );
        string k10text = getEdgarFiling( stock, *acn);

        Info info( stock._ticker(), last_year, StatementType::K10) ;
        extract10kToDisk( k10text, stock, info);

        createFourthQuarter( stock, last_year );
        updated = true;
    }
    else
    {
        cout << "\n No need to download" << endl;
        if (stock._eps( t._year() == last_year &&  
                        t._quarter() == 5 ).empty() )
        {
            createFourthQuarter( stock, last_year );
            updated = true;
        }
    }
    if (updated)
        createTtmEps( stock );
}

void
EdgarData::extract10kToDisk(string& k10, O_Stock& stock, Info& info){

    Parser parser = Parser();
    auto extracted_reports = new map<ReportType,string>;
    parser.extract_reports(k10, extracted_reports);
    
    // Save extracted reports
    _reports = *extracted_reports;

    //iterate over extracted_reports and write each one to disk
    for(auto it=extracted_reports->begin();it!= extracted_reports->end();++it)
    {
        if (it->first == ReportType::INCOME)
        {
            addAnualIncomeStatmentToDB(it->second, stock);
        }
        if (it->first == ReportType::BALANCE)
        {
            addBalanceStatmentToDB(it->second, stock);
        }
    }
}


void 
EdgarData::addAnualIncomeStatmentToDB(string& incomeFileStr, 
                                      O_Stock& stock,
                                      bool singleYear)
{
    Parser parser;    
    string incomeTableStr = parser.extractFirstTableStr(incomeFileStr); 
    
    // extract table into xml tree
    XmlElement* tree = parser.buildXmlTree(incomeTableStr);
    
    string units;
    string currency;
    vector<size_t> years = parser.titleInfo( tree, units, currency, singleYear);

    vector<string> revenues = parser.getRevenues(tree, singleYear);
    vector<string> incs = parser.getIncs(tree, singleYear);

    vector<float> eps = parser.getAnualEps(tree, singleYear);
    vector<string> shares = parser.getNumShares(tree,units);

    if ( shares.empty() )
    {
        LOG_INFO << "\n NO nushare data. proceding to get numshares from cover"
                 << " report. Which will have numshares for single year only";
        string coverReport = _reports[ReportType::COVER];
        if (coverReport =="")
        {
            LOG_ERROR << "No cover report found for " << stock._ticker()
                      << "So exusted attempt to read num shares";
            cout << "\n Cannot get num shares for " << stock._ticker() 
                 << ". exiting." << endl;
            return;

        }
        string sharesNum =      
            parser.getNumSharesFromCoverReport(_reports[ReportType::COVER]);
        if ( sharesNum != "")
        {
            shares.push_back(sharesNum);
            LOG_INFO << "Succeeded to get num shares from cover report. "
                     << "Only adding most recent year information to DB; "
                     << "numshares: "<< shares[0];
        }else{
            LOG_ERROR << "Failed to retrieve num shares from cover report"
                      << "Not going to add any info.";
        }
    }
    size_t numToAdd(1);
    if (!singleYear)
        numToAdd = std::min({ years.size(), revenues.size(), incs.size(), 
                    eps.size(), shares.size()});

    for (size_t i = 0; i < numToAdd; ++i)
        addEearningsRecordToDB( stock, years[i], 0/*quarter*/,
                                (revenues[i] + units),
                                (incs[i] + units),
                                eps[i], shares[i],
                                "edgar.com"/*source*/);
}

void
EdgarData::addBalanceStatmentToDB(string& incomeFileStr, 
                                  O_Stock& stock)
{
// #  current_assets      :string(255)
//#  total_assets        :string(255)
//#  current_liabilities :string(255)
//#  total_liabilities   :string(255)
//#  long_term_debt      :string(255)
//#  net_tangible_assets :string(255)
//-calculate:
//#  book_value          :string(255)




}
