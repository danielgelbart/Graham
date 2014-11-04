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

#include "Financials.h"

using namespace std;

// can be a utility function
void
EdgarData::write_to_disk(string& content, string& info, const path& writeDest)
{
    LOG_INFO << "writing to disk at " << writeDest.string();
    path dir = writeDest;
    path filePath = writeDest / info;
    LOG_INFO << "Writing to " << filePath;
    if (!exists(dir)){
        path partial;
        for (auto it = dir.begin(); it != dir.end(); ++it){
            LOG_INFO << "checking for existance of " << *it;
            if (*it != "."){
                partial /= *it;
                if (!exists(partial)){
                    LOG_INFO << "Creating directory " << partial;
                    create_directory(partial);
                    if (!exists(partial))
                        LOG_ERROR << "Could not create directory " 
                                  << partial.string();
                }
            }
        }
    }

    cout << "\n File path is: " << filePath << "\n";

    boost::filesystem::ofstream outFile(filePath);
    outFile << content;
    outFile.close();

    // use info to determine derectory path and file name       
    
    // base directrory is:
// write to ../../../reprots
    
    // remove first element (ticker) from string 

    // write file


}

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

void
EdgarData::extractBalance(){
// get the full download document
// find the 'doc list'
// locat the relavent doc which representst the balance statment
// 

    string fileName =  "../../text_files/IBM2013.txt";
    Parser parser = Parser();
    parser.extract_and_save_reports(fileName);


// extract ANY xpath tag

//close

}


