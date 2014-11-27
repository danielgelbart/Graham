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

void
EdgarData::downloadToString(Url& url, string& rContent)
{
    mHttpClient.httpGet(url.fullAddr(), "text/html", rContent);
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

void
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
 
    for(auto it = qAcns.begin() ; it != qAcns.end(); ++it)
    {
        cout << "\n Going to get Acn: " << (*it)->_acn << " date: " << 
            (*it)->_report_date << " quartr: " << (*it)->_quarter << endl;
        
        if ( ! stock_contains_acn( stock, *(*it) ) )
        {
            cout << "\n Stock does not have this data yet" << endl;
            addQuarterIncomeStatmentToDB( *(*it), stock );
        }
    }  
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
    downloadToString( url, page );

    Parser parser = Parser();
    string incomeStr = parser.extract_quarterly_income(page);

    incomeStr = parser.extractIncomeTableStr( incomeStr );
    XmlElement* tree = parser.buildXmlTree(incomeStr);

    string units;
    string currency;
    string revenue;
    string income;
    double eps;
    parser.parseQuarterlyIncomeStatment(tree, units, currency,
                                        revenue, income, eps);
    O_Ep incomeS( stock._id() );
    incomeS._year() = acn._report_date.year();
    incomeS._revenue() = revenue;
    incomeS._net_income() = income;
    incomeS._eps() = eps;
    incomeS._quarter() = acn._quarter;
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
         (ep._year() < greg_year(2012)) ||
         (ep._quarter() > 4 )           ||
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

    
// for testing porpuses
    getQuarters( stock );

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

        getQuarters( stock );

//        createFourthQfrom10k( stock );



    }
    else
        cout << "\n No need to download" << endl;

    // Get all 10-q from this year

}

void
EdgarData::extract10kToDisk(string& k10, O_Stock& stock, Info& info){

    Parser parser = Parser();
    auto extracted_reports = new map<ReportType,string>;
    parser.extract_reports(k10, extracted_reports);

    //iterate over extracted_reports and write each one to disk
    for(auto it=extracted_reports->begin();it!= extracted_reports->end();++it)
    {
        if (it->first == ReportType::INCOME)
        {
            addAnualIncomeStatmentToDB(it->second, stock, info);
        }
        if (it->first == ReportType::BALANCE)
        {
            //addBalanceStatmentToDB(it->second, info);
        }
    }
}

void 
EdgarData::addAnualIncomeStatmentToDB(string& incomeFileStr, 
                                      O_Stock& stock, 
                                      Info& info){
    Parser parser;    
    string incomeTableStr = parser.extractIncomeTableStr(incomeFileStr); 
    
    // extract table into xml tree
    XmlElement* tree = parser.buildXmlTree(incomeTableStr);
    
    // returns
    // tInfo[0] - units (bil or mil)
    // tInfo[1] - currency
    // tInfo[2,3,4..] years for wich data is given
    vector<string> tInfo = parser.titleInfo(tree);

    for(auto it = tInfo.begin(); it != tInfo.end(); ++it)
        cout << "\n " << *it << endl;

    size_t colNum = tInfo.size() - 2; 
    vector<size_t> years;

    for( size_t i = 2; i < colNum; ++i)
        years.push_back( stoi( tInfo[i] ) );

//    cout << "Title text is: " << titleText << endl;
    // get years, end date, and order of columns
  
    vector<string> revenues = parser.getRevenues(tree);
    vector<string> incs = parser.getIncs(tree);
    vector<float> eps = parser.getEps(tree);

    for (size_t i = 0; i < colNum; ++i)
    {
        O_Ep incomeS( stock._id() );
        incomeS._year() = years[i];
        incomeS._revenue() = revenues[i];
        incomeS._net_income() = incs[i];


// insertIncomeSTtoDB( incomeS );
//        incomeS.insert();

        //      cout << "\n inserted object " << incomeS._id() << endl;
    }    
}
