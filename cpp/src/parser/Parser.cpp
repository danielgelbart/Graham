#include <fstream>
#include <streambuf>
#include <iostream>
#include <map>

#include "Utils.hpp"

#include "Tokenizer.h"
#include "Parser.h"
/*
@fileName - a 10-k dump file from edgar.com

The file will be parsed for Income and Balance reports
They will be writen to disk
After all reprots are extracted, the dump file @fileName will be deleted
 */
void
Parser::extract_reports(string& fileName, 
                        map<ReportType,string>* extracted_reports)
{
    ifstream t(fileName);
    string str;

    // allocate memory first, as files are huge and therefore
    // this is quicker than realocting as you go
    t.seekg(0, std::ios::end);   
    str.reserve(t.tellg());
    t.seekg(0, std::ios::beg);

    // read the file into the string
    //NOTE: "the most vexing parse" - c011 has syntax to fix this using '{'
    str.assign( (istreambuf_iterator<char>(t) ),
               istreambuf_iterator<char>());
    
    Tokenizer tokenizer(str);

    string filingSummary = tokenizer.findFilingSummary();

    Tokenizer filingSummaryTok(filingSummary);
    auto reports = new map<ReportType,string>;
    filingSummaryTok.getReportDocNames(reports);

    //extract INCOME statement from dump file
    string reportKey;
    ReportType reportType = ReportType::INCOME;
    if ( (reports->find(reportType)) != reports->end() )
    {   
        reportKey = reports->find(reportType)->second;
        string docString = "<FILENAME>"+reportKey;
        string docFileString = tokenizer.findDoc(docString);
        extracted_reports->insert( pair<ReportType,string>( 
                                       reportType, 
                                       docFileString) );
    }

    //extract BALANCE statement from dump file
    reportType = ReportType::BALANCE;
    if ( (reports->find(reportType)) != reports->end() )
    {   
        reportKey = reports->find(reportType)->second;
        string docString = "<FILENAME>"+reportKey;
        string docFileString = tokenizer.findDoc(docString);
        extracted_reports->insert( pair<ReportType,string>( 
                                       reportType, 
                                       docFileString) );
    }
}


