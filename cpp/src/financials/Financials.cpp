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
EdgarData::getLastYear10KAcn( O_Stock& stock)
{
    string cik( to_string(stock._cik()) );
    string uri = "https://www.sec.gov/cgi-bin/browse-edgar?action=getcompany&CIK=" + cik + "&type=10-k&dateb=&owner=exclude&count=40";
    Url url = Url(uri);
//    Info info( stock._ticker(), last_year, StatementType::K10) ;
    string page;
    downloadToString( url, page);
    Parser parser;
    string acn = parser.extractLatest10kAcn(page);

    cout << " \n Got acn : " << acn << endl;
    return acn;
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
    Parser parser;
    string cik( to_string(stock._cik()) );
    string page;
    string uri = "http://www.sec.gov/cgi-bin/browse-edgar?action=getcompany&CIK=" + cik + "&type=10-q&dateb=&owner=exclude&count=40";
    Url url = Url(uri);
    
    downloadToString( url, page);
    vector<Acn*> qAcns = parser.getQuarterAcns( page );

    bool updated(false);
    for(auto it = qAcns.begin() ; it != qAcns.end(); ++it)
    {
        cout << "\n Going to get Acn: " << (*it)->_acn << " date: " << 
            (*it)->_report_date << " quartr: " << (*it)->_quarter << endl;
        
        if ( ! stock_contains_acn( stock, *(*it) ) )
        {
            cout << "\n Stock does not have this data yet" << endl;
            addQuarterIncomeStatmentToDB( *(*it), stock );
            updated = true;
        }
    }  
    return updated;
}    

void
EdgarData::addQuarterIncomeStatmentToDB(Acn& acn, O_Stock& stock)
{
    string cik( to_string(stock._cik()) );
    string uri("http://www.sec.gov/Archives/edgar/data/");
    uri += cik + "/" + acn._acn + ".txt";
    Url url = Url(uri);
    string page;
    
    // check for 404
    if (! downloadToString( url, page ) )
    {
        LOG_INFO << "\n failed to retrieve webpage." <<
            " NOT getting info for quarter " << to_string(acn._quarter) << "\n";
        return;
    }

    Parser parser = Parser();
    string incomeStr = parser.extract_quarterly_income(page);

    incomeStr = parser.extractFirstTableStr( incomeStr );
    XmlElement* tree = parser.buildXmlTree(incomeStr);

    string units;
    string currency;
    string revenue;
    string income;
    double eps;
    string numshares;
    parser.parseQuarterlyIncomeStatment(tree, units, currency,
                                        revenue, income, eps, numshares);
    O_Ep incomeS( stock._id() );
    incomeS._year() = acn._report_date.year();
    incomeS._quarter() = acn._quarter;
    incomeS._revenue() = revenue;
    incomeS._net_income() = income;
    incomeS._eps() = eps;
    incomeS._shares() = numshares;
    incomeS._source() = "edgar.com";
    incomeS._report_date() = acn._report_date.year();

    cout << " \n Going to insert to DB!" << endl;
    if ( ! insertEp( incomeS ) )
        cout << "\n Could not add earnings for quarter" << acn._quarter << "to DB" << endl;
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
    
    O_Ep fourth( stock._id());    
    fourth._year() = year;
    fourth._quarter() = 4;
    fourth._revenue() = to_string( revp );
    fourth._net_income() = to_string( incp );
    fourth._shares() = to_string(numshares);
    fourth._eps() = ((double)incp) / numshares;
    fourth._source() = "calculated";
    cout << "\n created fourth quarter with rev: " << to_string(revp) << " inc = " << to_string( incp ) << endl;
    if ( ! insertEp( fourth ) )
        cout << "\n Could not add calculated forth quarter" << endl;
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

    // print for sort inspection
    /* cout << "\n Sorted quarters for " << stock._ticker() << " are: " << endl;
    for( auto it = qrts.begin(); it != qrts.end(); ++it)
        cout << to_string(it->_year()) << " qrt- " << to_string(it->_quarter()) << endl;
    */

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
    
    O_Ep ttm( stock._id());    
    ttm._year() = qrts[0]._year();
    ttm._quarter() = 5;
    ttm._revenue() = to_string( revp );
    ttm._net_income() = to_string( incp );
    ttm._shares() = to_string(numshares);
    ttm._eps() = ((double)incp) / numshares;
    ttm._source() = "calculated";

    LOG_INFO << "\n created ttm income, with rev: " << to_string(revp) 
             << " inc = " << to_string( incp );

    if ( ! insertEp( ttm ) )
        LOG_ERROR << "\n Could not add calculated ttm data for " 
                  << stock._ticker();
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
        string acn10k = getLastYear10KAcn( stock );
        string uri = "http://www.sec.gov/Archives/edgar/data/" +
            cik + "/" + acn10k + ".txt";
        Url url = Url(uri);
        string k10text;
        downloadToString( url, k10text );
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
                                      O_Stock& stock)
{
    Parser parser;    
    string incomeTableStr = parser.extractFirstTableStr(incomeFileStr); 
    
    // extract table into xml tree
    XmlElement* tree = parser.buildXmlTree(incomeTableStr);
    
    bool onlyLatestYear(false);
    string units;
    string currency;
    vector<size_t> years = parser.titleInfo( tree, units, currency);
    vector<string> revenues = parser.getRevenues(tree);
    vector<string> incs = parser.getIncs(tree);
    vector<float> eps = parser.getAnualEps(tree);
    vector<string> shares = parser.getNumShares(tree,units);

    size_t numToAdd(1);

    if ( shares.empty() )
    {
//        LOG_INFO << "\n cover report is: \n" << _reports[ReportType::COVER];
        onlyLatestYear = true;
        string sharesNum =      
            parser.getNumSharesFromCoverReport(_reports[ReportType::COVER]);
        sharesNum = removeNonDigit( sharesNum );
        if ( sharesNum != "")
        {
            shares.push_back(sharesNum);
            LOG_INFO << "Succeeded to get num shares from cover report. Only adding most recent year information to DB; numshares: "<< shares[0];
        }else{
            LOG_ERROR << "Failed to retrieve num shares from cover report"
                      << "Not going to add any info.";
            numToAdd = 0;
        }
    }

    if (!onlyLatestYear)
        numToAdd = years.size();

    cout << "Adding data for " << to_string(numToAdd) << " years.\n";
    cout << "Size of years vector is " << to_string( years.size());
    for (size_t i = 0; i < numToAdd; ++i)
    {
        cout << " I'm still going1\n";
        O_Ep incomeS( stock._id() );
        incomeS._year() = years[i];
        cout << " I'm still going2\n";
        incomeS._quarter() = 0;    
        cout << " I'm still going3\n";
        incomeS._revenue() = revenues[i] + units;
        incomeS._net_income() = incs[i] + units;
        cout << " I'm still going4\n";
        incomeS._eps() = eps[i];
        incomeS._shares() = shares[i];
        incomeS._source() = "edgar.com";
        LOG_INFO << " \n Going to try to inser to DB, annual eps data for year "
                 << to_string(years[i]);
        if ( ! insertEp( incomeS ) )
            LOG_ERROR << "\n Failed to insert to DB earnings for year" 
                      << to_string(years[i]) << ". ";
    }    
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
