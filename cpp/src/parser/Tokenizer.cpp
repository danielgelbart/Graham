#include <vector>
#include <map>
#include <iostream>

#include <boost/regex.hpp>
//#include <regex>

#include "Logger.h"
#include "Tokenizer.h"

//Returns empty string if no match
string
Tokenizer::getNextDelString(string& delimiter)
{
    size_t nextPos = _text.find(delimiter,_pos+1);

    if (nextPos == string::npos)
        return "";

    size_t lastPos = _pos;
    _pos = nextPos;

    return _text.substr(lastPos, (nextPos-lastPos));
}

//Returns string::npos==-1 if no match
size_t
Tokenizer::getNextDocPos()
{
    string delimiter("<DOCUMENT>");
    size_t del_length = delimiter.length();
    size_t nextPos = _text.find(delimiter,_pos+del_length);

    if (nextPos != string::npos)
        _pos = nextPos;

    return nextPos;
}

void
Tokenizer::parseToDocuments()
{
    //extract all '<DOCUMENT>' start pos into a vector
    size_t pos = 0;
    while(pos != string::npos)
    {
        _doc_poses.push_back(pos);
        pos = getNextDocPos();
    }
}

string
Tokenizer::findFilingSummary()
{
    parseToDocuments();
    string filingString("<FILENAME>FilingSummary.xml");

    //iterate over docs in REVERSE
    //for each one, check if it is filing summary
    bool FLfound(false);
    for (auto rit = _doc_poses.rbegin(); rit!= _doc_poses.rend(); ++rit)
    {
        if (_text.find(filingString,*rit) != string::npos)
        {       
            FLfound = true;
            // set member _pos to point to the filing summary
            _pos = *rit;
            break;
        }           
    }                      
    if (!FLfound){
        LOG_ERROR << "Could NOT locate FilingSummary\n" ;
    }

    string delimiter("<DOCUMENT>");
    return getNextDelString(delimiter);
}
string
Tokenizer::findDoc(string& docName)
{
    bool FLfound(false);
    for (auto it = _doc_poses.begin(); it!= _doc_poses.end(); ++it)
    {
        if (_text.substr(*it,400).find(docName) != string::npos)
        {       
            FLfound = true;
            // set member _pos to point to the filing summary
            _pos = *it;
            break;
        }           
    }                      
    if (!FLfound){
        LOG_ERROR << "Could NOT locate document" << docName <<"\n" ;
    }

    string delimiter("<DOCUMENT>");
    return getNextDelString(delimiter);
}

string
Tokenizer::extractTagContent(string& tagName, string& content)
{
    string openTag("<"+tagName+">");
    string closeTage("</"+tagName+">");

// boost is supposed to allow access to the named subexpression NAME
// but I don't know how
    boost::regex pattern(openTag+"(?<NAME>(.*))("+closeTage+"){1,1}");
    boost::smatch match;
    //  cout << "\n <>Contstructed pattern is " << pattern << endl;
 
    boost::regex_search(content, match, pattern);

    return match[1];
}

string
Tokenizer::readReportHtmlNameFromFS(string& report)
{
    string doc_tag("HtmlFileName");
    string doc_name = extractTagContent(doc_tag,report);

    if (doc_name == "")
    {
        doc_tag = "XmlFileName";
        doc_name = extractTagContent(doc_tag,report);
    }
    return doc_name;
}

void
Tokenizer::getReportDocNames(map<ReportType,string>* reports_map)
{
    bool foundIncomeRep(false);
    bool foundBalanceRep(false);
    string delimiter("<Report>");
    string tagName("ShortName");
    string report = "";
    string reportName = "";

    boost::regex income_pattern(
        "(\\s)*consolidated statement of (earnings|income)(\\s)*",
        boost::regex_constants::icase);
    boost::regex balance_pattern(
        "(\\s)*consolidated (statement of financial position|balance sheet)(\\s)*",
        boost::regex_constants::icase);

    while( (report = getNextDelString(delimiter)) != "")
    {
        reportName = extractTagContent(tagName,report);

        // TODOcheck that name does NOT inclue (Parenthetical)

        cout << "\nName: " << reportName << endl;
        if (boost::regex_match(reportName, income_pattern))
            {
                foundIncomeRep = true;
                cout << "FOUND INCOME MATCH!!\n"<< reportName << endl;

                reports_map->insert( pair<ReportType,string>(
                                        ReportType::INCOME,
                                        readReportHtmlNameFromFS(report)) ); 
            }
        if (boost::regex_match(reportName, balance_pattern))
        {
            foundBalanceRep = true;
            cout << "FOUND BALANCE MATCH!!\n"<< reportName << endl;

            reports_map->insert( pair<ReportType,string>(
                                     ReportType::BALANCE,
                                     readReportHtmlNameFromFS(report)) ); 
        }

        if (foundBalanceRep && foundIncomeRep)
            break;
    }
}

