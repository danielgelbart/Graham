#ifndef PARSER_H
#define PARSER_H

#include <boost/filesystem.hpp>   
#include <map>

#include "types.h"
#include "Tokenizer.h"

using namespace std;

enum class XmlTokenType {OPEN, CLOSE, TEXT, IGNORE };

class XmlElement {
public:
    XmlElement(string& tagName)
        {
            _tagName = tagName;
            _text = "";
        }
    
    void addChild(XmlElement* child);
    void addAttr(string& xml);
    void addText(string& text);
    
    string text();

    void getNodes(string tagName, //with no leading or trailng whitespace 
                  size_t number, // the number of such xml elements to collect
                  vector<XmlElement*>* collected);

public: //members
    string              _tagName;
    vector<XmlElement*> _children;
    map<string,string>  _attributes;
    string              _text;
}; // end class XmlElement

class Parser {
public:
    Parser() {}

    XmlElement* buildXmlTree(string& xmlDocument);
    void parseXML(XmlElement* node, Tokenizer& tok);

    void extract_reports(string& k10, 
                         map<ReportType,string>* extract_reports);
    string extractIncomeTableStr(string& incomeStr);

    vector<string> titleInfo(XmlElement* tree);

private:

    XmlTokenType tokenType( string& xml);

// members

};

#endif //PARSER





