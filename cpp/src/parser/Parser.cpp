#include <fstream>
#include <streambuf>
#include <iostream>
#include <map>
#include <boost/regex.hpp>

#include "Utils.hpp"

#include "Tokenizer.h"
#include "Parser.h"

void
XmlElement::addChild( XmlElement* child )
{
    _children.push_back(child);
}

void
XmlElement::addAttr( string& xml )
{
    //string test_str = "<a href=\"javascript:void(0);\" onclick=\"top.Show.toggleNext( this );\"> ";
    
    // split sting by " " (space delimiter)
    boost::regex pattern( "(?<KEY>(\\w*))=\"(?<VALUE>(.*))\"\\s");

    // use an iterator!
    boost::sregex_iterator mit(xml.begin(), xml.end(), pattern);
    boost::sregex_iterator mEnd;

    for(; mit != mEnd; ++mit)
    {
        string key = (*mit)[1].str();
        string value = (*mit)[3].str();

        cout << "\n Found attr: " << key << " = " << value << endl;
        _attributes.insert( pair<string,string>(key,value) );
    }
}

void
XmlElement::addText( string& xml )
{
    _text += xml;
}



XmlTokenType
Parser::tokenType( string& xml)
{
    if ( xml == "<br>") 
        return XmlTokenType::IGNORE;
    
    if ( xml.at(0) == '<')
    {
        if ( xml.at(10) == '/')
        {
            return XmlTokenType::CLOSE;
        }
        return XmlTokenType::OPEN;
    } else 
        return XmlTokenType::TEXT;
}


XmlElement* 
Parser::buildXmlTree(string& xmlDocument)
{
    Tokenizer xmlTok = Tokenizer(xmlDocument);
    
    string rootName("__ROOT__");
    XmlElement* root = new XmlElement(rootName);
        
    parseXML( root, xmlTok);

    return root;
}

// returns the name of and xml open tag
string
extract_tag_name(string& xmlOpenTag)
{
    size_t endPos = xmlOpenTag.find(" ",0);
    if (endPos == string::npos)
        return xmlOpenTag.substr(1, xmlOpenTag.length() - 2);
    return xmlOpenTag.substr(1, endPos - 1);
}

void
Parser::parseXML(XmlElement* node, Tokenizer& tok){

    string xml_token = tok.xmlNextTok();

    // need to declare before case switch
    string name;
    XmlElement* child;

    switch ( tokenType(xml_token) )
    {
    case XmlTokenType::IGNORE :
        parseXML( node, tok );
        break;

    case XmlTokenType::OPEN :
        name =  extract_tag_name(xml_token);
        child = new XmlElement( name );
        node->addChild( child );
        node->addAttr( xml_token );
        parseXML( child, tok );
        break;

    case XmlTokenType::CLOSE :
        return;

    case XmlTokenType::TEXT :
        node->addText( xml_token );
        parseXML( node, tok);
        break;

    default :
        ;

    }

}




/*
@fileName - a 10-k dump file from edgar.com

The file will be parsed for Income and Balance reports
They will be writen to disk
After all reprots are extracted, the dump file @fileName will be deleted
 */
void
Parser::extract_reports(string& k10, 
                        map<ReportType,string>* extracted_reports)
{
    /*
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
    
    */

    Tokenizer tokenizer(k10);

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


