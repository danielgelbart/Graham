#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/filesystem/fstream.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "Config.h"
#include "Url.h"
#include "Logger.h"
#include "types.h"
#include "dmmm_dbface.h"
#include "T_Stock.hpp"
#include "T_Ep.hpp"
#include "T_Note.hpp"
#include "Financials.h"
#include "Parser.h"
#include <boost/regex.hpp>

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
    LOG_INFO << "Called getEdgarFiling()\n";
    string cik( to_string(stock._cik()) );
    string uri("http://www.sec.gov/Archives/edgar/data/");
    uri += cik + "/" + acn._acn + ".txt";
    Url url = Url(uri);
    string page("");
    LOG_INFO << "Created url string\n";
    // check for 404
    if (! downloadToString( url, page ) )
    {
        LOG_ERROR << "\n failed to retrieve webpage." <<
            " NOT getting info for quarter " << to_string(acn._quarter) << "\n";
        return "";
    }
    LOG_INFO << "downloaded page\n";
    if (page == "")
        LOG_ERROR << "Something went wrong, could not get edgar filing "<<
            acn._acn <<" for cik " << stock._cik() << "\n";
    return page;
}

Acn*
EdgarData::getLastYear10KAcn( O_Stock& stock)
{
    string page = getEdgarSearchResultsPage( stock , StatementType::K10);
    LOG_INFO << "Got search results for "<<stock._ticker()<<"\n";
    Parser parser;
    vector<Acn*> Acns = parser.getAcnsFromSearchResults( page, 1,/*limit*/ 
                                                         StatementType::Q10 );
    if (Acns.empty())
    {
        LOG_ERROR << "NO ACNS FOR "<<stock._ticker()<<"\n";
        return NULL;
    }
    LOG_INFO << "Got acn "<<Acns.front()->_acn;
    return Acns.front();
}

bool
stock_contains_acn(O_Stock& stock, Acn& acn)
{
    vector<O_Ep> epss = stock._eps();
    for (auto it = epss.begin() ; it != epss.end(); ++it )
        if ( it->_year()    == acn._year &&
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
            addSingleQuarterIncomeStatmentToDB(page,stock, (*it)->_report_date.year(),
                                               (*it)->_quarter);
            updated = true;
        }
    }  
    return updated;
}    

bool
EdgarData::addEarningsRecordToDB( O_Stock& stock, O_Ep& incomeS)
{
    bool inserted(false);
    stringstream message;
    message << "Record to DB for " << stock._ticker()
            <<". YEAR: "<< incomeS._year() << " Q: "<< incomeS._quarter()
            <<" REV: "<<incomeS._revenue()<<" INC: "<<incomeS._net_income()
            <<" SHARES: "<<incomeS._shares()<<" EPS: "<<incomeS._eps() <<"\n";
       
    if ( (inserted = insertEp(incomeS)) )
    {
        LOG_INFO << "\nEntered NEW Earnings "<< message.str();
        cout << "\nEntered NEW Earnings "<<message.str() << endl;
    } else {
        LOG_ERROR << "\n FAILED to insert to DB earnings "<<message.str(); 
        cout << "\nFAILED to insert to DB earnings "<<message.str() << endl;
    }
    return inserted;
}

bool
EdgarData::addEarningsRecordToDB( O_Stock& stock, size_t year, size_t quarter,
                                   string revenue, string income, float eps,
                                   string shares, const string& source)
{
    LOG_INFO << "\nGoing to try to insert earnings record to DB for "
             << stock._ticker() << " for year " << to_string(year) 
             << " quarter " << to_string(quarter);
    O_Ep incomeS( stock._id() );
    incomeS._year() = year;
    incomeS._quarter() = quarter;    
    incomeS._revenue() = revenue;
    incomeS._net_income() = income;
    incomeS._eps() = eps;
    incomeS._shares() = shares;
    incomeS._source() = source;
        
    return addEarningsRecordToDB(stock, incomeS);
}

bool
EdgarData::insertEp( O_Ep& ep )
{
    // check that ep is valid
// Add checks for existance of :
// eps
// num shares
    if ( (ep._stock_id() <= 0 )         || 
         (ep._year() < greg_year(1980)) ||
         (ep._year() > greg_year(2020)) ||
         (ep._quarter() > 5 )           ||
         (ep._quarter() < 0 )
        )
        return false;

    T_Stock ta;
    I_Stock stock_id( ep._stock_id() );
    pair<O_Stock, bool> spair = ta.select( stock_id );
    if (!spair.second)
    {
        LOG_ERROR<<"Stock with id "<<to_string(ep._stock_id())<<" could not be"
                  <<" found in DB. Therefore, cannot add earnings for it to DB";
        return false;
    } 
    O_Stock stock = spair.first;
   
    // check if it already exits in DB
    date dd( ep._year(),Jan,1);
    Acn acn( string("dummy"), dd, ep._quarter() );
    acn._year = ep._year();
    if ( stock_contains_acn( stock, acn ) )
    {
        LOG_INFO<<"NOT adding earnings to DB. Record for that quarter/year "
                <<"already exists";
        return false;
    }

    // Add notes on failure, if needed
    if ( (ep._revenue() == "")            ||
         (ep._net_income() == "")         ||
         (withinPercent(ep._eps(),0.1,0.0)) ||
         (ep._shares() =="")
         ){
        O_Note note;
        note._stock_id() = stock._id();
        note._year() = ep._year();

        if (ep._revenue() == ""){
            note._pertains_to() = EnumNotePERTAINS_TO::NO_REV;
            string message = "Could not retrieve revenue";
            note._note() = message;
            LOG_INFO << "Added NOTE - "<< message ;
        }
        if (ep._net_income() == ""){
            note._pertains_to() = EnumNotePERTAINS_TO::NO_INC;
            string message = "Could not retrieve Net Income";
            note._note() = message;
            LOG_INFO << "Added NOTE - "<< message ;
        }
        if (withinPercent(ep._eps(),0.1,0.0)){
            note._pertains_to() = EnumNotePERTAINS_TO::NO_EPS;
            string message = "Could not retrieve EPS";
            note._note() = message;
            LOG_INFO << "Added NOTE - "<< message ;
        }
        if (ep._shares() == ""){
            note._pertains_to() = EnumNotePERTAINS_TO::NO_SHARES;
            string message = "Could not retrieve shares outstanding";
            note._note() = message;
            LOG_INFO << "Added NOTE - "<< message ;
        }
        // insert note for missing data
        note.insert();
    }
    // insert earnings
    return ep.insert();
}

bool
EdgarData::addBalanceRecordToDB( O_Stock& stock, O_BalanceSheet& balance_rec)
{
    bool inserted(false);
    stringstream message;
    message << " BALANCE Record to DB for " << stock._ticker()
            <<". YEAR: "<< balance_rec._year() <<" CA: "<<balance_rec._current_assets()
            <<" TA: "<<balance_rec._total_assets() << " CL: "<<balance_rec._current_liabilities()
            <<" TL: "<<balance_rec._total_liabilities() << " TLD: " << balance_rec._long_term_debt()
            <<" BV: "<<balance_rec._book_value() << "\n";

    if ( (inserted = insertBs(balance_rec)) )
    {
        LOG_INFO << "\nInserted NEW "<< message.str();
        cout << "\nInserted NEW "<<message.str() << endl;
    } else {
        LOG_ERROR << "\n FAILED to insert "<<message.str();
        cout << "\nFAILED to insert "<<message.str() << endl;
    }
    return inserted;
}

bool
EdgarData::insertBs( O_BalanceSheet& bs )
{
    // check that bs is valid
// Add checks for existance of :
// bs
// num shares
    if ( (bs._stock_id() <= 0 )         ||
         (bs._year() < greg_year(1980)) ||
         (bs._year() > greg_year(2020))
        )
        return false;

    T_Stock ta;
    I_Stock stock_id( bs._stock_id() );
    pair<O_Stock, bool> spair = ta.select( stock_id );
    if (!spair.second)
    {
        LOG_ERROR<<"Stock with id "<<to_string(bs._stock_id())<<" could not be"
                  <<" found in DB. Therefore, cannot add earnings for it to DB";
        return false;
    }
    O_Stock stock = spair.first;

    // check if it already exits in DB
    vector<O_BalanceSheet> bss = stock._balance_sheets();
    for (auto it = bss.begin() ; it != bss.end(); ++it )
        if ( it->_year() == bs._year() )
        {
            LOG_INFO<<"NOT adding balance statment to DB. Record for that year already exists";
            return false;
        }

    // insert new balance record
    return bs.insert();
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
    addEarningsRecordToDB( stock, year, 4,/*quarter*/
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

    addEarningsRecordToDB( stock, qrts[0]._year(), 5,/*quarter*/
                            to_string(revp), to_string(incp),
                            ((double)incp) / numshares, /*EPS*/
                            to_string(numshares),
                            "calculated");
}

// move to Dates.hpp?
bool
reportWithin3Months(date rep_date, date endDate)
{
    LOG_INFO << "\n Looking for report end date of "<<to_simple_string(endDate)
             << "\n Report handled is from "<<to_simple_string(rep_date);
 
    date_period dp( endDate, (endDate + months(3)) );
    return ( dp.contains( rep_date ) );
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
    Acn* acn = NULL;
    for(auto it = Acns.begin() ; it != Acns.end(); ++it)
    {
        LOG_INFO << "\n Got Acn: " << (*it)->_acn << " date: " << 
            (*it)->_report_date << " quartr: " << (*it)->_quarter;
        
        if ( ! stock_contains_acn( stock, *(*it) ) )
        {
            date end_date = calculateEndDate(stock._fiscal_year_end(),year,0);
            if(reportWithin3Months( (*it)->_report_date, end_date) ) 
            {
                acn = *it;
                LOG_INFO << "Got ACN for specific year " << to_string(year);
                break;
            }
        }
    }  
    if (acn == NULL)
    {
        LOG_INFO << "Could not get acn for "<<stock._ticker() << " for "
                 <<" "<<to_string(gyear) << "\n";
        return false;
    }
// get Specific one for year to a string
    string filing = getEdgarFiling(stock,*acn);
    if (filing == "")
        return false;
// extract to DB
    //Info info( stock._ticker(), gyear, StatementType::K10);
    extract10kToDisk(filing, stock, year);

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

    date today = day_clock::local_day();
    greg_year last_year = today.year() - 1;

    bool updated(false);
    //updated = getQuarters( stock );

    // check if last years 10k exists
    T_Ep t;
    if (stock._eps( t._year() == last_year &&  
                    t._quarter() == 0 ).empty() )
    {
        cout << "\n Going to get last year 10k" << endl;
        Acn* acn  = getLastYear10KAcn( stock );
        string k10text = getEdgarFiling( stock, *acn);
        extract10kToDisk( k10text, stock, last_year);

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
EdgarData::extract10kToDisk(string& k10, O_Stock& stock, size_t year){

    Parser parser = Parser();
    auto extracted_reports = new map<ReportType,string>;
    parser.extract_reports(k10, extracted_reports);
    
    // Save extracted reports
    _reports = *extracted_reports;

    if (_reports.size() == 0)
        return;

    string income_report = _reports[ReportType::INCOME];
    if (income_report == "")
        LOG_ERROR << "MISING INCOME REPORT";
    else
        addSingleAnualIncomeStatmentToDB(income_report, stock, year);
    string balance_report = _reports[ReportType::BALANCE];
    if (balance_report == "")
        LOG_ERROR << "MISING BALANCE REPORT";
    else
        addBalanceStatmentToDB(balance_report, stock, year);
}

void 
EdgarData::addSingleAnualIncomeStatmentToDB(string& incomeFileStr, 
                                            O_Stock& stock, size_t year)
{
    Parser parser;
    parser.set_stock(stock);

    string incomeTableStr = parser.extractFirstTableStr(incomeFileStr); 
    XmlElement* tree = parser.buildXmlTree(incomeTableStr);
    if(tree == NULL)
    {
        LOG_INFO << "No income statement to parse to DB for "<<stock._ticker();
        return;
    }
    O_Ep ep;
    bool shares_outstanding(false);
    ep._stock_id() = stock._id();
    ep._year() = year;
    ep._quarter() = 0; // Anual record
    ep._source() = string("edgar.com");
    parser.parseIncomeTree(tree, ep);
    if (ep._shares() == "")
    {
        parser.getNumSharesFromCoverReport( _reports[ReportType::COVER], ep );
        shares_outstanding = true;
    }

    if ( (withinPercent(ep._eps(),0.01,0.0)) &&
         (ep._shares() != "" ) &&
         (ep._net_income() != "") )
    {
        long income = stol(ep._net_income());
        long shares = stol(ep._shares());

        double eps = (double)income/shares;
        ep._eps() = eps;

        O_Note note;
        note._stock_id() = stock._id();
        note._year() = ep._year();
        note._pertains_to() = EnumNotePERTAINS_TO::INCOME_REP;
        note._note() = "EPS calculated using shares and income";
        note.insert();
        LOG_INFO << "Calculated eps to be "<< to_string(ep._eps());
    }

    if ( addEarningsRecordToDB( stock, ep) &&
         shares_outstanding )
    {
        O_Note note;
        note._stock_id() = stock._id();
        note._year() = ep._year();
        note._pertains_to() = EnumNotePERTAINS_TO::SHARES_OUTSTANDING;
        note._note() = "using shares outstanding from cover report";
        note.insert();
    }


    // for testing
    _ep = ep;
}

void
EdgarData::addSingleQuarterIncomeStatmentToDB(string& filing,
                                            O_Stock& stock, size_t year, size_t quarter)
{
    Parser parser;
    parser.set_stock(stock);

    string incomeStr = parser.extract_quarterly_income(filing);
    incomeStr = parser.extractFirstTableStr( incomeStr );
    XmlElement* tree = parser.buildXmlTree(incomeStr);

    if(tree == NULL)
    {
        LOG_INFO << "No income statement to parse to DB for "<<stock._ticker();
        return;
    }

    O_Ep ep;
    ep._stock_id() = stock._id();
    ep._year() = year;
    ep._quarter() = quarter; // Anual record
    ep._source() = string("edgar.com");
    parser.parseIncomeTree(tree, ep);

    if ( (withinPercent(ep._eps(),0.01,0.0)) &&
         (ep._shares() != "" ) &&
         (ep._net_income() != "") )
    {
        long income = stol(ep._net_income());
        long shares = stol(ep._shares());

        double eps = (double)income/shares;
        ep._eps() = eps;

        O_Note note;
        note._stock_id() = stock._id();
        note._year() = ep._year();
        note._pertains_to() = EnumNotePERTAINS_TO::INCOME_REP;
        note._note() = "QUARTER: " + to_string(quarter) +" |EPS calculated using shares and income";
        note.insert();
        LOG_INFO << "Calculated eps to be "<< to_string(ep._eps());
    }

    // for testing
    _ep = ep;
}


bool 
EdgarData::getFiscalYearEndDate(O_Stock& stock)
{
    LOG_INFO << "fyed() called";
    Acn* acn = getLastYear10KAcn(stock);
    LOG_INFO << "Got acn\n";
    if (acn == NULL)
    {
        LOG_INFO << "Did not get acn for "<<stock._ticker() ;
        return false;
    }
    string filing = getEdgarFiling(stock,*acn);
    if (filing == "")
    {
        LOG_ERROR << "Failed to retrive filing for acn "<<acn->_acn<<"\n";
        return false;
    }
    LOG_INFO << "Got filing\n";
    Parser parser;

    // clear reports
    _reports.clear();
    // load new rports
    parser.extract_reports(filing, &_reports);

    LOG_INFO << "Got "<<_reports.size()<<" reports\n";
    ReportType reportType = ReportType::COVER;
    auto coverReportIt = _reports.find(reportType);
    if( coverReportIt == _reports.end())
        return false;
    LOG_INFO << "Got cover report\n";

    string date = parser.extractFiscalDateFromReport(coverReportIt->second);
    if (date == "")
    {
        LOG_INFO << "Failed to get fiscal year end date from cover report";
        return false;
    }
    cout << "Adding fyed: "<< date<<" to "<<stock._ticker()<<endl;
    stock._fiscal_year_end() = date;
    stock.update();
    return true;
}

void
EdgarData::loadCountryMaps()
{
    path binPath = confParam<string>("argv0");
    path filePath = binPath.remove_filename() /
            "../../text_files/country_codes.txt";

    cout << "going to load file at " << filePath.string();
    string filing =  loadFileToString(filePath.string());

    std::ifstream infile(filePath.string());

    std::string line;
    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        string a, b;
        if (!(iss >> a >> b))
        { break; } // error
        if (a == "Canadian")
            break;
        usmap[a] = b;
        // process pair (a,b)
    }

    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        string a, b;
        if (!(iss >> a >> b))
        { break; } // error
        if (a == "Other")
            break;
        camap[a] = b;
        // process pair (a,b)
    }

    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        string a, b;
        if (!(iss >> a >> b))
        { break; } // error
        if (a == "Canadian")
            break;
        omap[a] = b;
        // process pair (a,b)
    }
}



bool
EdgarData::getCountry(O_Stock& stock)
{
    string cik( to_string(stock._cik()) );
    string statType = "10-k";
    string uri = "http://www.sec.gov/cgi-bin/browse-edgar?action=getcompany&CIK=" + cik + "&type=" +
            statType + "&dateb=&owner=exclude&count=40";

    Url url = Url(uri);
    string page;
    downloadToString( url, page);

    string tagname("State of Inc.: <strong>");
    size_t pos = page.find(tagname) ;
    string country_code;
    if (pos != string::npos)
        country_code = page.substr( pos + tagname.length(), 2);

    string country;
    if (usmap[country_code] != "")
        country = "USA";
    else
        if (camap[country_code] != "")
            country = "CANADA";
        else
            country = omap[country_code];

    cout << "Got " << country << " for " << stock._ticker() <<
            " for country code " << country_code << endl;

    if (country != ""){
        stock._country() = country;
        stock.update();
    }
    return true;
}


void
EdgarData::addBalanceStatmentToDB(string& balanceFileStr,
                                  O_Stock& stock, size_t year)
{
// #  current_assets      :string(255)
//#  total_assets        :string(255)
//#  current_liabilities :string(255)
//#  total_liabilities   :string(255)
//#  long_term_debt      :string(255)
//#  net_tangible_assets :string(255)
//-calculate:
//#  book_value          :string(255)
    Parser parser;
    parser.set_stock(stock);

    string balanceTableStr = parser.extractFirstTableStr(balanceFileStr);

    XmlElement* tree = parser.buildXmlTree(balanceTableStr);
    if(tree == NULL)
    {
        LOG_INFO << "No BALANCE statement to parse to DB for "<<stock._ticker();
        return;
    }
    O_BalanceSheet bs;
    bs._stock_id() = stock._id();
    bs._year() = year;
    bs._quarter() = 0; // Anual record

    parser.parseBalanceTree(tree, bs);

    addBalanceRecordToDB ( stock, bs);
    //_bs = bs;
}
