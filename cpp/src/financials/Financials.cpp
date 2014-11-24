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
    // add tests for failure to retrieve?
    mHttpClient.httpGet(url.fullAddr(), "text/html", rContent);
}


void
EdgarData::downloadAndSave(Url& url, Info& info, const path& writeDest)
{
    string rContent; // will hold the returned text

    // add tests for failure to retrieve?
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
    
    cout << "\n Retrived acn quartryly data as follows :\n" << endl;

    for(auto it = qAcns.begin() ; it != qAcns.end(); ++it)
        cout << "\n Acn: " << (*it)->_acn << " date: " << 
            (*it)->_report_date << " quartr: " << (*it)->_quarter << endl;
    // for each do -

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
        // Get last year 10k as well as 4th quarter
        string acn10k = getLastYear10KAcn( stock );
        string uri = "http://www.sec.gov/Archives/edgar/data/" +
            cik + "/" + acn10k + ".txt";
        Url url = Url(uri);
        Info info( stock._ticker(), last_year, StatementType::K10) ;
        string k10text;
        downloadToString( url, k10text );
        extractFinantialStatementsToDisk( k10text, info );
        parseStatementsToDB();

        getQuarters( stock );
//        createFourthQfrom10k( stock );
    }
    else
        cout << "\n No need to download" << endl;

    // Get all 10-q from this year

}

// Shoud be extract finantial statments from 10-k dump file down load
// Pass path to downloaded file
void
EdgarData::extractFinantialStatementsToDisk(string& k10, Info& info){

    path fileDest = FINANCIALS_PATH / info.ticker;
    string fileNameStr = info.ticker + "_" + to_string(info.year) + "_";

    Parser parser = Parser();
    auto extracted_reports = new map<ReportType,string>;
    parser.extract_reports(k10, extracted_reports);

    //iterate over extracted_reports and write each one to disk
    for(auto it=extracted_reports->begin();it!= extracted_reports->end();++it)
    {
        if (it->first == ReportType::INCOME)
        {
            string infoString(fileNameStr+"income.txt");
            write_to_disk(it->second, infoString, fileDest);
        }
        if (it->first == ReportType::BALANCE)
        {
            string infoString(fileNameStr+"balance.txt");
            write_to_disk(it->second, infoString, fileDest);
        }
    }
    
    // Cretate fourth quarter from 10k - 3 x 10q

}




O_Stock
getStock(string ticker)
{
    T_Stock ta;
    return ta.select( ta._ticker() == ticker ).front();
}


void 
EdgarData::parseStatementsToDB(){
 
    // load file from disk into string
    string incomeFilePath( FINANCIALS_PATH );
    incomeFilePath += "/IBM/IBM_2013_income.txt";
    string incomeFileStr = loadFileToString(incomeFilePath);

    string ticker = "IBM";
    T_Stock ta;
    O_Stock stock =  ta.select( ta._ticker() == ticker ).front() ;

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

//        incomeS.insert();

        //      cout << "\n inserted object " << incomeS._id() << endl;
    }    
}
