#ifndef PARSER_H
#define PARSER_H

#include <boost/filesystem.hpp>   
#include <map>

#include "types.h"

using namespace std;

class Parser {
public:
    Parser() {}

    void extractBalance();
    void extract_reports(string& k10, 
                         map<ReportType,string>* extract_reports);
    
private:
// members

};

#endif //PARSER





