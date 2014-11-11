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

#include "Financials.h"

using namespace std;



void
printXmlTree(XmlElement* node, size_t depth)
{
    for(size_t i = 0; i < depth; ++i)
        cout << "  ";
    
    cout << "< " << node->_tagName; 

    if ( node->_attributes.empty() )
        cout << " (no attrs) ";
    else
    {
        cout << " Has attrs: \n";
        for(auto it = node->_attributes.begin(); 
            it != node->_attributes.end(); ++it)
            cout << "\t" << it->first << " = " << it->second << endl;
    }
    for(size_t i = 0; i < depth+1; ++i)
        cout << "  ";
    if ( node->_text == "" )
        cout << " Contains no text" << endl;
    else
        cout << " Content is: " << node->_text << endl;

    for( auto it = node->_children.begin(); it != node->_children.end(); ++it )
        printXmlTree( *it, (depth+1) );
}

void
EdgarData::downloadAndSave10k(Url& url, Info& info)
{
    const path& writeDest = FINANCIALS_PATH / info.ticker;
    cout << "\n  Path to store file is: " << writeDest << endl;
    downloadAndSave(url,info,writeDest);
}

void
EdgarData::downloadToString(Url& url, Info& info, string& rContent)
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

/*
Download the latest available 10-k for a specific ticker
save only the extracted financial statments to disk
*/
void 
EdgarData::updateFinancials(){
    LOG_INFO << "updateFinancials method was called \n";

// get current date
    // check if last 4 quarters exist
    // check if 10k is available
// open interned connection and download 

//* 4th quarter needs to be retrieved from 10-k 
    Info info;   
    info.ticker = "IBM";
    info.year = 2013; // get from current time
    info.type = StatementType::K10;
        
    // IBM 10-q 2nd 2014
    // string uri("http://www.sec.gov/Archives/edgar/data/51143/000005114314000007/0000051143-14-000007.txt");

    // IBM 2013 10-k
    string uri("http://www.sec.gov/Archives/edgar/data/51143/000104746914001302/0001047469-14-001302.txt");
    //string info("IBM_10-q_2nd_2014");
    
    Url url = Url(uri);
//    downloadAndSave10k(url, info);

    //replace with:
    string k10;
    downloadToString( url, info, k10 );
    extractFinantialStatementsToDisk( k10, info );
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
}

void 
EdgarData::parseStatementsToDB(){

    string test_str = 
        "<div> class=\"body\" style=\"padding: 2px;\">                                     <a>- Definition                                                                </a>                                                                           <div>                                                                            <p>The aggregate cost of goods produced and sold and services rendered during the reporting period.                                                           </p>                                                                         </div>                                                                         <a>+ References                                                                </a>                                                                           <div style=\"display: none;\">                                                   <p>Reference 1: http://www.xbrl.org/2003/role/presentationRef<br>ticle 5<br><br><br><br>                                                                      </p>                                                                         </div>                                                                       </div>";

        Parser parser;
//        string docTable = parser.extractBalanceFromFile();

        XmlElement* tree = parser.buildXmlTree(test_str);
        
        //test
        printXmlTree(tree,0);

        // O_Income = new
        
}
