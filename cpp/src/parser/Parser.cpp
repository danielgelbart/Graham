#include <fstream>
#include <streambuf>
#include <iostream>
#include <map>

#include "Parser.h"
#include "Tokenizer.h"

void
Parser::extract_and_save_reports(string& fileName){

    ifstream t(fileName);
    string str;

    // allocate memory first, as files are huge and therefore
    // this is quicker than realocting as you go
    t.seekg(0, std::ios::end);   
    str.reserve(t.tellg());
    t.seekg(0, std::ios::beg);

    // read the file into the string
    str.assign( (istreambuf_iterator<char>(t) ),
               istreambuf_iterator<char>());
    
    Tokenizer tokenizer(str);

 

    // find Filing summary
    string filingSummary = tokenizer.findFilingSummary();

    Tokenizer filingSummaryTok(filingSummary);

    map<ReportType,string>* reports = new map<ReportType,string>;
    filingSummaryTok.getReportDocNames(reports);

    cout << "\n\nReport names are: "<< endl;
    for(auto it = reports->begin(); it != reports->end(); ++it)
    {
        cout << "\n->" << it->second << endl;
    }

    string reportKey;
    if ( (reports->find(ReportType::INCOME)) != reports->end() )
    {   
        reportKey = reports->find(ReportType::INCOME)->second;
        tokenizer.extractIncome(reportKey);
    }

//    tokenizer.extract_balace();
}


