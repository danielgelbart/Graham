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
#include "O_ShareClass.hpp"
#include "Financials.h"
#include "Dates.hpp"
#include <cmath>
#include <boost/regex.hpp>
//#inclue "stockhelper.h"

using namespace std;
using namespace DMMM;
using namespace boost;


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

    string uri = "https://www.sec.gov/cgi-bin/browse-edgar?action=getcompany&CIK=" + cik + "&type=" + statType + "&dateb=&owner=exclude&count=40";

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
    string uri("https://www.sec.gov/Archives/edgar/data/");
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

/* Return the Acn record for the most recent available 10k record */
Acn*
EdgarData::getLastYear10KAcn( O_Stock& stock)
{
    string page = getEdgarSearchResultsPage( stock , StatementType::K10);
    LOG_INFO << "Got search results for "<<stock._ticker()<<"\n";
    Parser parser(stock);
    //parser.set_stock(stock);
    vector<Acn*> Acns = parser.getAcnsFromSearchResults( page, 0,/*limit*/
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
    bool has_ep(false);
    bool has_bs(false);
    vector<O_Ep> epss = stock._eps();
    for (auto it = epss.begin() ; it != epss.end(); ++it )
        if ( it->_year()    == acn._year &&
             it->_quarter() == acn._quarter )
            has_ep = true;
    if (acn._quarter == 0){
        vector<O_BalanceSheet> bss = stock._balance_sheets();
        for (auto it = bss.begin() ; it != bss.end(); ++it )
            if ( it->_year()    == acn._year )
                has_bs = true;
    } else
        has_bs = true;
    return (has_ep && has_bs);
}

bool
EdgarData::get_report_year_date_quarter(string cover_rep, Acn* acn_p){
    // convert cover_rep string to tree
    string cover_table = _parser.extractFirstTableStr( cover_rep );
    XmlElement* tree = _parser.buildXmlTree( cover_table );

    if(tree == NULL)
    {
        LOG_INFO << "Failed to build tree for cover report from stirng: \n" << cover_rep;
        return false;
    }
    // extract year for - check if matches - set on Acn
    trIterator trIt(tree);
    XmlElement* trp = tree;
    string yearStr(""), quarterStr("");
    int year = -1, quarter = -1;
    regex yearfor_pattern("Document Fiscal Year Focus", regex::icase);
    regex quarterfor_pattern("Document Fiscal Period Focus", regex::icase);

    while( (trp = trIt.nextTr()) != NULL )
    {
        string trtext = trp->text();

        if (regex_search(trtext, yearfor_pattern))
        {
            boost::regex year_pattern("\\d\\d\\d\\d");
            boost::smatch match;
            if (boost::regex_search(trtext, match, year_pattern) )
            {
                yearStr = match[0];
                LOG_INFO << "Report is for year "<< yearStr;
                year = stoi(yearStr);

            } else
                LOG_ERROR << "Could not get the year the report is for";
        }

        if (regex_search(trtext, quarterfor_pattern))
        {
            boost::regex quarter_pattern("Q\\d");
            boost::smatch match;
            if (boost::regex_search(trtext, match, quarter_pattern) )
            {
                quarterStr = match[0];
                LOG_INFO << "Report is for quarter "<< quarterStr;
                quarter = (quarterStr[1] - '0');

            } else
                LOG_ERROR << "Could not get the quarter the report is for";
        }
    }
    if (( year > 0) && ( quarter > 0 )){
        acn_p->_year = year;
        acn_p->_quarter = quarter;
        LOG_INFO << "Set acn year and quarter to: "
                  << to_string(acn_p->_year) <<"/"<<to_string(acn_p->_quarter);
    } else {
        LOG_ERROR << "Could not retrieve year and quarter from cover report";
        return false;
    }
    return true;
}


bool
EdgarData::check_report_year_and_date(string cover_rep, Acn* acn_p){
    // convert cover_rep string to tree
    string cover_table = _parser.extractFirstTableStr( cover_rep );
    XmlElement* tree = _parser.buildXmlTree( cover_table );

    if(tree == NULL)
    {
        LOG_INFO << "Failed to build tree for cover report from stirng: \n" << cover_rep;
        return false;
    }
    // extract year for - check if matches - set on Acn
    trIterator trIt(tree);
    XmlElement* trp = tree;
    string yearStr(""), quarterStr("");
    int year = -1, quarter = -1;
    regex yearfor_pattern("Document Fiscal Year Focus", regex::icase);
    regex quarterfor_pattern("Document Fiscal Period Focus", regex::icase);

    while( (trp = trIt.nextTr()) != NULL )
    {
        string trtext = trp->text();

        if (regex_search(trtext, yearfor_pattern))
        {
            boost::regex year_pattern("\\d\\d\\d\\d");
            boost::smatch match;
            if (boost::regex_search(trtext, match, year_pattern) )
            {
                yearStr = match[0];
                LOG_INFO << "Report is for year "<< yearStr;
                year = stoi(yearStr);

            } else
                LOG_ERROR << "Could not get the year the report is for";
        }

        if (regex_search(trtext, quarterfor_pattern))
        {
            boost::regex quarter_pattern("Q\\d");
            boost::smatch match;
            if (boost::regex_search(trtext, match, quarter_pattern) )
            {
                quarterStr = match[0];
                LOG_INFO << "Report is for quarter "<< quarterStr;
                quarter = (quarterStr[1] - '0');

            } else
                LOG_ERROR << "Could not get the quarter the report is for";
        }
    }
  // compare to acn, if different  - write error to log?
    if ( (acn_p->_year != year) || (acn_p->_quarter != quarter) )
        LOG_ERROR << "DEBUG: found mistake in evaluating quarter of report from date "
                  << acn_p->_report_date
                  << ". Using report date and fyed, Evaluated year/Q where  - "
                  << to_string(acn_p->_year) <<"/"<<to_string(acn_p->_quarter)
                  <<"\n while report itself says it is for year/Q - "
                  << yearStr <<"/"<<quarterStr;

    //set acn year and quarter regardless
    /* MY assesmet of quarter is BETTER than the companies filings
     * they have mistakes! example 2014/Q3 for T, dated Nov-10th-2014 is marked at bing for Q2!!!
     * And At&T (T)'s report for the first quarter, is marked as for quarter 'FY' !!!!
     * So for now, I am usnig my own evalution, over companies filings
    if (year > 0)
        acn_p->_year = year;
    if (quarter > 0)
        acn_p->_quarter = quarter;
     */

    // return true just means we succeeded to check with no failure
    return true;
}


bool
EdgarData::getAnnuals(O_Stock& stock)
{
    _parser.set_stock(stock);
    string page = getEdgarSearchResultsPage(stock,StatementType::K10);
    vector<Acn*> qAcns = _parser.getAcnsFromSearchResults( page, 4,/*limit*/
                                                          StatementType::K10 );
    bool updated(false);
    for(auto it = qAcns.begin() ; it != qAcns.end(); ++it)
    {
        std::stringstream msg;
        msg << "\n Looking at Annual Acn dated: " << (*it)->_report_date
                << " quartr: " << to_string((*it)->_quarter) << " ";
        LOG_INFO << msg.str();
        cout << msg.str() << endl;

        if ( ! stock_contains_acn( stock, *(*it) ) )
        {
            cout << "\n Getting Annual report dated " << (*it)->_report_date
                 << " for year "<< (*it)->_year << endl;
            string page = getEdgarFiling( stock, *(*it));

            bool gotAnnual = extract10kToDisk( page, stock, (*it)->_year );
            if (gotAnnual)
                createFourthQuarter( stock,(*it)->_year );
            updated = (updated || gotAnnual);
        } else {
            string message = "Not going to download, since stock already contains this info";
            LOG_INFO << message;
            cout << message << " for year/Q: " << (*it)->_year << "/" << (*it)->_quarter << endl;
        }
    }
    return updated;
}


bool
EdgarData::getQuarters(O_Stock& stock)
{
    // get back to Q1 LAST year
    _parser.set_stock(stock);

    // Make sure that fiscal year end date is set for stock
    if (stock._fiscal_year_end().length() < 4){
        LOG_ERROR << "NO Fiscal year end set for stock. Cannot procede to evaluate quarters";
        return false;
    }

    //StockHelper stockH(stock);
    //if (stockH.quartersUpToDate())
    //    return true;

    string page = getEdgarSearchResultsPage(stock,StatementType::Q10);

    /* limit number of reports to get
     * limit == 0 limits to last year
     * NOTE: staticly set HERE
     */
    size_t limit = 7;
    vector<Acn*> qAcns = _parser.getAcnsFromSearchResults( page, limit,/*limit*/
                                                          StatementType::Q10 );
    // Iterate over ALL retrieved ACNs
    // Try to add, if not already in DB
    bool updated(false);
    for(auto it = qAcns.begin() ; it != qAcns.end(); ++it)
    {
        LOG_INFO << "\n Looking at Acn: " << (*it)->_acn << " date: " <<
            (*it)->_report_date << " quartr: " << (*it)->_quarter;

        if ( ! stock_contains_acn( stock, *(*it) ) )
        {
            cout << "\n Getting Qaurter report dated " << (*it)->_report_date << endl;
            string page = getEdgarFiling( stock, *(*it));

            auto extracted_reports = new map<ReportType,string>;
            _parser.extract_reports(page, extracted_reports);
            if( extracted_reports->empty() )
            {
                LOG_ERROR << "Could not extract individual reports from filing";
                continue;
            }
            _reports = *extracted_reports;
            string cover_rep = _reports[ReportType::COVER];
            if (cover_rep != "")
                check_report_year_and_date(cover_rep, *it);
            else
                LOG_ERROR << "NO COVER REPORT";
            string income_rep = _reports[ReportType::INCOME];
            if (income_rep != ""){
                addIncomeStatmentToDB( income_rep, stock, (*it)->_year, (*it)->_quarter,
                                       cover_rep, to_iso_extended_string((*it)->_report_date) );
                updated = true;
            } else
                LOG_ERROR << "NO INCOME REPORT!!!";
        } else {
            string message = "Not going to download, since stock already contains this info";
            LOG_INFO << message;
            cout << message << " for year/Q: " << (*it)->_year << "/" << (*it)->_quarter << endl;
        }
        _reports.clear();
    }  
    return updated;
}    

bool
EdgarData::getSingleQarter(O_Stock& stock, string acc_num){
     _parser.set_stock(stock);

     Acn* acn = new Acn();
     acn->_acn = acc_num;

     string page = getEdgarFiling( stock, *acn);

     auto extracted_reports = new map<ReportType,string>;
     _parser.extract_reports(page, extracted_reports);
     if( extracted_reports->empty() )
     {
         LOG_ERROR << "Could not extract individual reports from filing";
         return false;
     }

     _reports = *extracted_reports;
     string cover_rep = _reports[ReportType::COVER];
     if (cover_rep == ""){
         LOG_ERROR << "Could not extract cover report for single quarter filing";
         return false;
     }

     if (!get_report_year_date_quarter(cover_rep,acn)){
         return false;
     }

     string income_rep = _reports[ReportType::INCOME];
     if (income_rep != ""){
         addIncomeStatmentToDB( income_rep, stock, acn->_year, acn->_quarter, cover_rep, "0"/*date of filing missing*/);
     } else
         LOG_ERROR << "NO INCOME REPORT!!!";

     _reports.clear();
    return true;
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

    //If record is empty - do not enter it
    if( ( ep._revenue() == "")     &&
            ( ep._net_income() == "" ) &&
            ( ep._shares() == "" )     &&
            withinPercent(ep._eps(),0.1,0.0) ){
            LOG_ERROR<<" Earnings record is empty (contains no data). Not going to insert into DB";
            return false;
    }


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
   
    vector<O_Ep> epss = stock._eps();
    for (auto it = epss.begin() ; it != epss.end(); ++it )
        if ( it->_year()    == ep._year() &&
             it->_quarter() == ep._quarter() )
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

greg_year
getLatestAnnualEpYear(O_Stock& stock){
    greg_year year = 1980;
    auto epss = stock._eps();
    for(auto it = epss.begin(); it != epss.end(); ++it )
      if (( it->_quarter() == 0) && (it->_year() > year))
      {
          //cout << "Year is " << it->_year() << endl;
          year = it->_year();
      }
    cout << "\nMost recent year for "<< stock._ticker() << " is " << year;
    return year;
}

void
EdgarData::createFourthQuarter(O_Stock& stock, size_t year)
{
    //first check that stock has latest annual report + 3 quarter reports of same year
    T_Ep t;
    vector<O_Ep> epss = stock._eps( (t._year() == year) && (t._quarter() == 0) );
    if (epss.empty())
    {
            LOG_ERROR << "Could not retrive any annual income records for year " << to_string(year);
            return;
    }
    O_Ep annual_ep = epss.front();

    vector<O_Ep> incs = stock._eps( (t._year() == year) && (t._quarter() > 0) && (t._quarter() < 4));
    if ( incs.size() != 3 )
    {
        LOG_ERROR << "Cannot create fourth quarter. Missing quarters data for " << stock._ticker()
                     << " for year " << annual_ep._year() << ", Only have " << to_string(incs.size()) <<
                       " quarters ";
        return;
    }

    long revp = 0, incp = 0, numshares = 0;
    // if rev, inc, or share data is missing, stol() will throw an exception
    try{
        revp = stol(annual_ep._revenue());
        incp = stol(annual_ep._net_income());
        numshares = stol(annual_ep._shares());

        for ( auto it = incs.begin(); it != incs.end(); ++it)
        {
            revp -= stol( it->_revenue() );
            incp -= stol( it->_net_income() );
        }
    }
    catch (const std::invalid_argument& ia) {
        cout << "\nin createFourthQuarter() stol() threw Invalid argument: " << ia.what() << '\n';
        LOG_ERROR << "missing data to calcualte createFourthQuarter() for year "<< year <<". cannot do it";
        return;
    }

    // if fourth quarter for year already exists, delete it before ading newly calculated one
    if (! stock._eps( (t._year() == year) && (t._quarter() == 4) ).empty())
    {
        LOG_INFO << " Deleting old fourth quarter record for year " << to_string(year)
                    << " before adding new record to DB";

        string table("eps");
        string where = "stock_id=" + to_string(stock._id())
            +" AND year=" + to_string(year) +" AND quarter=4";
        DBFace::instance()->erase(table,where);
    }

    addEarningsRecordToDB( stock, year, 4,/*quarter*/
                            to_string( revp ),/*revenue*/
                            to_string( incp ),/*income*/
                            ((double)incp) / numshares, /* EPS */
                            to_string(numshares),  
                            "calculated"/*source*/);

    createTtmEps(stock);

}

struct ep_comp {
    bool operator() (const O_Ep& i, const O_Ep& j) const
        { 
        if ( i._year() == j._year() )
            return ( i._quarter() > j._quarter() );
        else
            return (i._year() > j._year() );
        }
} ep_comp;

void
EdgarData::createTtmEps(O_Stock& stock)
{
    // delete last ttm
    string table("eps");
    string where = "stock_id=" + to_string(stock._id())+" AND quarter=5";
    DBFace::instance()->erase(table,where);

    T_Ep t;
    // get all quarter reports, and SORT - NEWEST FIRST
    vector<O_Ep> qrts = stock._eps( (t._quarter() > 0) &&
                                    (t._quarter() < 5));
    if ( qrts.size() < 4 )
    {
        LOG_ERROR << " Stock does not have enough quarters."
             << " Cannot procede to calculate ttm eps";
        return;
    }
    // if latest quarter is annual, create ttm_eps record, but
    // simply copy all data from annual record
    sort( qrts.begin(), qrts.end(), ep_comp );

    O_Ep latest_annual_ep = t.select( (t._stock_id() == stock._id()) &&
                                      (t._quarter() == 0) &&
                                      (t._year() == getLatestAnnualEpYear(stock)) ).front();

    if ( (qrts[0]._year() == latest_annual_ep._year()) &&
         (qrts[0]._quarter() == 4 ) )
    {
        LOG_INFO << "Annual report for "<< latest_annual_ep._year() << " is the newest earnings recod"
                    << " So setting ttm record (quarter 5) to be identical";
        addEarningsRecordToDB( stock, qrts[0]._year(), 5,/*quarter*/
                                latest_annual_ep._revenue(), latest_annual_ep._net_income(),
                                latest_annual_ep._eps(), latest_annual_ep._shares(),"calculated");
        return;
    }

    // sum latest 4 quarters
    long revp(0);
    long incp(0);
    bool check[4] = {false,false,false,false};

    try{
        for( size_t i=0 ; i < 4; ++i)
        {
            revp += stol( qrts[i]._revenue() );
            incp += stol( qrts[i]._net_income() );
            check[ qrts[i]._quarter() - 1 ] = true;
        }
    } catch (const std::invalid_argument& ia) {
        cout << "\nin createTtmEps(), stol() threw Invalid argument: " << ia.what() << '\n';
        LOG_ERROR << "missing data to calcualte createTtmEps(). cannot do it";
        return;
    }

    if ( !(check[0] && check[1] && check[2] && check[3]) )
    {
        LOG_ERROR << " Mising a quarter for " << stock._ticker() <<
            " cannot procede to calculate ttm eps";
        return;
    }
    long numshares = stol( qrts[0]._shares() );

    LOG_INFO << "Finishe doint ttm earnings. Got quarters: " << qrts[0]._year() << " Q"<< qrts[0]._quarter() << "\n"
                 << qrts[1]._year() << " Q"<< qrts[1]._quarter() << "\n"
                     << qrts[2]._year() << " Q"<< qrts[2]._quarter() << "\n"
                         << qrts[3]._year() << " Q"<< qrts[3]._quarter() << "\n"
          << "Calculated values are: total Rev"<< to_string(revp) << " total inc: " << to_string(incp)
             << " numshares " << to_string(numshares);

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
    parser.set_stock(stock);
    vector<Acn*> Acns = parser.getAcnsFromSearchResults( page, 10,/*limit*/ 
                                                         StatementType::K10 );
    greg_year gyear(year);
    Acn* acn = NULL;
    for(auto it = Acns.begin() ; it != Acns.end(); ++it)
    {
        LOG_INFO << "\n Got Acn: " << (*it)->_acn << " date: " << 
            (*it)->_report_date << " year: " << (*it)->_year << " quartr: " << (*it)->_quarter;
        
        if ( ((*it)->_quarter == 0) && ( ! stock_contains_acn( stock, *(*it))) )
        {
            //date end_date = calculateEndDate(stock._fiscal_year_end(),year,0);
            //if(reportWithin3Months( (*it)->_report_date, end_date) )
            if( year == (*it)->_year )
            {
                acn = (*it);
                LOG_INFO << "Got ACN for specific year " << to_string(year);
                break;
            }
        }
    }  
    if (acn == NULL)
    {
        LOG_INFO << "Either Could not get acn for "<<stock._ticker() << " for "
                 <<" "<<to_string(gyear) << ", OR stock already has this data in DB\n";
        return false;
    }
// get Specific one for year to a string
    string filing = getEdgarFiling(stock,*acn);
    if (filing == "")
        return false;


    if (!extract10kToDisk(filing, stock, year))
    {
        LOG_INFO << "Could not add annual report for "<< year;
        return false;
    }

    return retVal;
}

/* Retrieves all years avaliable (usualy 3) from a single income statment
 *
 */
void
EdgarData::getAllYearsFromIncome(O_Stock& stock, string acc_num)
{
    _parser.set_stock(stock);

    Acn* acn = new Acn();
    acn->_acn = acc_num;

    //raw filing
    string page = getEdgarFiling( stock, *acn);

    auto extracted_reports = new map<ReportType,string>;
    _parser.extract_reports(page, extracted_reports);
    _reports = *extracted_reports;

    string income_rep = _reports[ReportType::INCOME];
    XmlElement* tree = _parser.convertReportToTree(income_rep);

    if (tree != NULL){

        years_list* ylist = _parser.findAllAnnualColumnsToExtract(tree);

        _parser.extractMultipleYearsIncomeData(tree,ylist);

        //add to DB
        for(auto ep_it = ylist->begin(); ep_it != ylist->end(); ++ep_it)
            addEarningsRecordToDB( stock, *((*ep_it)->second) );

    } else
        LOG_ERROR << "NO INCOME REPORT!!!";

    _reports.clear();
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

    _parser = Parser(stock);



//    date today = day_clock::local_day();
//    greg_year last_year = today.year() - 1;

    bool updated(false);

    updated = getQuarters( stock );

    updated = (getAnnuals( stock ) || updated);

    if (updated)
        createTtmEps( stock );
}

bool
EdgarData::extract10kToDisk(string& k10, O_Stock& stock, size_t year){

    Parser parser = Parser();
    parser.set_stock(stock);
    auto extracted_reports = new map<ReportType,string>;
    parser.extract_reports(k10, extracted_reports);
    
    // Save extracted reports
    _reports = *extracted_reports;

    if (_reports.size() == 0)
        return false;

    string cover_report = _reports[ReportType::COVER];
    if (cover_report == "")
        LOG_ERROR << "MISING COVER REPORT";
    else {
        int* focus_year = new int(0), *year_end = new int(0);
        string* date_end = new string("");
        LOG_INFO << "\n COVER report is "<< cover_report;
        parser.extractFiscalDatesFromReport(cover_report, focus_year, date_end, year_end);
        parser.updateFiscalDates(stock, focus_year, date_end, year_end);
        //LOG_INFO << "Test print UPDATE: focus year: "<< *focus_year
        //       << " fyed: "<< *date_end << " current period end year: "<< *year_end;

        if (*date_end != "") {
            if( stock._fiscal_year_end() == ""){
                LOG_INFO << "Updating "<<stock._ticker() <<"'s fiscal year end date to "<<*date_end;
                stock._fiscal_year_end() = *date_end;
                stock.update();
            } else {
                if ( withinAweek(*date_end, stock._fiscal_year_end(),*focus_year)){
                    LOG_INFO << "Updating "<<stock._ticker() <<"'s fiscal year end date to "<<*date_end;
                    // Note that the following is will update even if there is no need to
                    stock._fiscal_year_end() = *date_end;
                    stock.update();
                } else {
                    O_Note note;
                    note._stock_id() = stock._id();
                    note._year() = year;
                    note._pertains_to() = EnumNotePERTAINS_TO::INCOME_REP;
                    string message = " Fiscal year end date not clear: previous year end was " +
                            stock._fiscal_year_end() + " but new 10k states it as " + *date_end;
                    note._note() = message;
                    note.insert();
                    LOG_INFO << "Added the following note to "<< stock._ticker() << " : "<< message;
                }
            }
        }
        if ( (stock._fy_same_as_ed() == true)
             && (*focus_year != 0) && (*year_end != 0) && (*date_end != "")
             && ((*date_end) != "12-31")
             && ((*focus_year) != (*year_end)) )
        {
            string message = "Setting " + stock._ticker() +
                    " to have focus year not correlat with end of period year fd_same_as_ey = false."
                    + " end date for stock is " + stock._fiscal_year_end();
            LOG_INFO << "NOTE " << message;
            cout << "! "<< message;
            stock._fy_same_as_ed() = false;
            stock.update();

            // if updated -> then we have the wrong report!
            return false;
        }
    }
    string income_report = _reports[ReportType::INCOME];
    if (income_report == "")
        LOG_ERROR << "MISING INCOME REPORT";
    else
        addIncomeStatmentToDB(income_report, stock, year, 0 /*annual*/, cover_report, "0" /*lost date of report*/);
    string balance_report = _reports[ReportType::BALANCE];
    if (balance_report == "")
        LOG_ERROR << "MISING BALANCE REPORT";
    else
        addBalanceStatmentToDB(balance_report, stock, year);
    return true;
}

void
EdgarData::addIncomeStatmentToDB(string& incomeStr, O_Stock& stock,
                                              size_t year, size_t quarter,
                                              string& cover_report, string report_date)
{
     _parser.set_stock(stock);
     XmlElement* tree = _parser.convertReportToTree(incomeStr);

    if(tree == NULL)
    {
        LOG_INFO << "No income statement to parse to DB for "<<stock._ticker();
        return;
    }
    date rep_end_date = _parser.extractPeriodEndDateFromCoverReport(cover_report);
    if (report_date == "0")
        report_date = to_iso_extended_string(rep_end_date);

    O_Ep ep;
    ep._stock_id() = stock._id();
    ep._year() = year;
    ep._quarter() = quarter;
    ep._source() = string("edgar.com");
    ep._report_date() = report_date;
    _parser.parseIncomeTree(tree, ep, rep_end_date);
    postParseEarningsFix( stock, ep);
}

void
EdgarData::postParseEarningsFix( O_Stock& stock, O_Ep& ep)
{
     bool shares_outstanding(false);
    // try to get num shares from cover report
    // try to calculate num shares from EPS and Income
    if (ep._shares() == "")
    {
        if (!_parser.getNumSharesFromCoverReport( _reports[ReportType::COVER], ep )){
            if ( (ep._net_income() != "") && ( abs(ep._eps()) > 0.01) )
            {
                long income = stol(ep._net_income());
                double shares_f = income / ep._eps() ;
                int shares_int = (int)(shares_f + 0.5);
                LOG_INFO << " Got numshares from calculating EPS and Net Income: "<< shares_int;
                ep._shares() = to_string(shares_int);
            }
        } else
            shares_outstanding = true;
    }

    // try to calculate EPS from num shares and net income
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

    // inesrt record to DB
    if ( addEarningsRecordToDB( stock, ep) &&
         shares_outstanding )
    {
        T_Note nt;
        if (nt.select(nt._stock_id() == stock._id()
                      && nt._year() == ep._year()
                      && nt._pertains_to() == EnumNotePERTAINS_TO::SHARES_OUTSTANDING).empty() )
        {
            O_Note note;
            note._stock_id() = stock._id();
            note._year() = ep._year();
            note._pertains_to() = EnumNotePERTAINS_TO::SHARES_OUTSTANDING;
            note._note() = "using shares outstanding from cover report";
            note.insert();
        }
    }
    // for testing
    _ep = ep;
}

bool 
EdgarData::getFiscalYearEndDate(O_Stock& stock)
{
    LOG_INFO << "fyed() called for "<< stock._ticker();
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
    parser.set_stock(stock);

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

    string* date_end = new string("");
    int* focus_year = new int(0), *year_end = new int(0);
    //string& report, int* focus_year, string* date_end, int* year_end)
    parser.extractFiscalDatesFromReport(coverReportIt->second, focus_year, date_end, year_end);
    parser.updateFiscalDates(stock, focus_year, date_end, year_end);

    return (stock._fiscal_year_end() != "");
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

    // construct from year end of stock
    date rep_end_date(greg_year(year),
                      greg_month(stoi(stock._fiscal_year_end().substr(0,2))),
                      greg_day(stoi(stock._fiscal_year_end().substr(3,2))));
    parser.parseBalanceTree(tree, bs, rep_end_date);

    addBalanceRecordToDB ( stock, bs);
    //_bs = bs;
}
