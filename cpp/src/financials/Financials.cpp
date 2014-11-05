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
EdgarData::downloadAndSave(Url& url, string& info, const path& writeDest)
{
    string rContent; // will hold the returned text

    // add tests for failure to retrieve?
    mHttpClient.httpGet(url.fullAddr(), "text/html", rContent);

    write_to_disk(rContent, info, writeDest);
}

void 
EdgarData::updateFinancials(){
    LOG_INFO << "updateFinancials method was called \n";
    
    // check what needs to be downladed

    // IBM 10-q 2nd 2014
    string uri("http://www.sec.gov/Archives/edgar/data/51143/000005114314000007/0000051143-14-000007.txt");
    string info("IBM_10-q_2nd_2014");
    Url url = Url(uri);
    path reportsDir = "../../reports";
    downloadAndSave(url, info, reportsDir);

// get current date
// retreive most recent annual repor
// open interned connection and download 
}


// Shoud be extract finantial statments from 10-k dump file down load
// Pass path to downloaded file
void
EdgarData::extractFinantialStatementsToDisk(path& fileName){
// get path to 10k on disk

    string fileSource =  "../../text_files/IBM2013.txt";
    path fileDest = "../../financials/IBM/";
    string info = "IBM_2013_";
    Parser parser = Parser();
    auto extracted_reports = new map<ReportType,string>;
    parser.extract_reports(fileSource,extracted_reports);

    //iterate over extracted_reports and write each one to disk
    for(auto it=extracted_reports->begin();it!= extracted_reports->end();++it)
    {
        if (it->first == ReportType::INCOME)
        {
            string infoString(info+"income.txt");
            write_to_disk(it->second, infoString, fileDest);
        }
        if (it->first == ReportType::BALANCE)
        {
            string infoString(info+"balance.txt");
            write_to_disk(it->second, infoString, fileDest);
        }
    }
}


