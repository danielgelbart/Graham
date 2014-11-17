#include <fstream>
#include <streambuf>
#include <iostream>
#include <map>
#include <boost/regex.hpp>

#include "Utils.hpp"

#include "Tokenizer.h"
#include "Parser.h"

void
XmlElement::printXmlTree(size_t depth)
{
    for(size_t i = 0; i < depth; ++i)
        cout << "  ";
    
    cout << "< " << _tagName; 
/*
    if ( _attributes.empty() )
        cout << " (no attrs) ";
    else
    {
        cout << " Has attrs: \n";
        for(auto it = _attributes.begin(); 
            it != _attributes.end(); ++it)
            cout << "\t" << it->first << " = " << it->second << endl;
    }
*/
    for(size_t i = 0; i < depth+1; ++i)
        cout << "  ";
    if ( _text == "" )
        cout << " Contains no text" << endl;
    else
        cout << " Content is: " << _text << endl;

    for( auto it = _children.begin(); it != _children.end(); ++it )
        (*it)->printXmlTree( depth+1);
}


void
XmlElement::addChild( XmlElement* child )
{
    _children.push_back(child);
}

void
XmlElement::addAttr( string& xml )
{
    boost::regex pattern( "(\\w+)=\"(.+?)\"");

    boost::sregex_iterator mit(xml.begin(), xml.end(), pattern);
    boost::sregex_iterator mEnd;

    for(; mit != mEnd; ++mit)
    {
//        for(size_t i = 0 ; i < mit->size() ; ++i)
//            cout << "\n Match " << to_string(i) << " is : " << (*mit)[i].str() << endl;
        string key = (*mit)[1].str();
        string value = (*mit)[2].str();
        //      cout << "\n Found attr: " << key << " = " << value << endl;
        _attributes.insert( pair<string,string>(key,value) );
    }
}

void
XmlElement::addText( string& xml )
{
    _text += xml;
}

XmlElement* 
XmlElement::tagWithText(string& tagName, string& phrase)
{
    boost::regex pattern( phrase );

    if (_tagName == tagName)
    {
        if ( boost::regex_search( text(), pattern ) )
            return this;
    }
    if ( _children.empty() )
        return NULL;
    
    XmlElement* node = NULL;
    for( auto it = _children.begin(); it != _children.end(); ++it )
    {
        node = (*it)->tagWithText( tagName,phrase );
        if ( node != NULL )
            break;
    }
    return node;
}

string
XmlElement::text()
{
    string rText = "";

    if ( _text != "" )
        rText += " : " + _text;
 
    for( auto it = _children.begin(); it != _children.end(); ++it )
        rText += (*it)->text();

    return rText;
}

XmlTokenType
Parser::tokenType( string& xml)
{
    boost::regex blank_string("\\s+");

    if ( ( xml == "<br>") ||
         ( boost::regex_match(xml, blank_string) ) )
        return XmlTokenType::IGNORE;
    
    if ( xml.at(0) == '<')
    {
        if ( xml.at(1) == '/')
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
    string name = xmlOpenTag.substr(1, endPos - 1);
    return trimSpaces(name);
}

void
Parser::parseXML(XmlElement* node, Tokenizer& tok){

    string xml_token;

    // need to declare before case switch
    string name;    
    XmlElement* child;

//TODO -- does not build correctly from 10k of IBM

    while ( ! tok.atEnd() )
    {
        xml_token = tok.xmlNextTok();
        //       cout << "\n procesing token: " << xml_token << endl;

        switch ( tokenType(xml_token) )
        {
        case XmlTokenType::OPEN :
            name =  extract_tag_name(xml_token);
            child = new XmlElement( name );
            node->addChild( child );
            child->addAttr( xml_token );
            parseXML( child, tok );
            break;

        case XmlTokenType::CLOSE :
            return;

        case XmlTokenType::TEXT :
            node->addText( xml_token );
            continue;

        case XmlTokenType::IGNORE :
            continue;

        default :
            ;

        } // switch
    } // while
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


string 
Parser::extractIncomeTableStr(string& incomeStr){
    
    string openTab("<table");
    string closeTab("</table");
    size_t startPos = incomeStr.find(openTab, 0);
    size_t endPos = incomeStr.find(closeTab, startPos);
    return incomeStr.substr( startPos , (endPos-startPos) );
}

vector<string> 
Parser::titleInfo(XmlElement* tree){
    vector<string> rInfo;       
    vector<XmlElement*>* elements = new vector<XmlElement*>;
    string tagName("tr");
    tree->getNodes(tagName, 2, elements);
 
    string titleText  = (*elements)[0]->text() + (*elements)[1]->text() ;
    
    cout << "\n Title text is : " << titleText << endl; 
    
    // get units
    string units(MILL);
    boost::regex pattern ("In Thousands");
    if (boost::regex_search(titleText, pattern) )
        units = THOU;
    pattern = "In Millions";
    if (boost::regex_search(titleText, pattern) )
        units = MILL;
    pattern = "In Billions";
    if (boost::regex_search(titleText, pattern) )
        units = BILL;
    rInfo.push_back(units);

    // get currency
    string currency = "USD";
    pattern = "USD";
    if (!boost::regex_search(titleText, pattern) )
        currency = "OTHER";
    rInfo.push_back(currency);
    
    // get dates
    boost::regex datePat("(\\w\\w\\w). (\\d+)?, (\\d+)?");
    boost::sregex_iterator mit(titleText.begin(), titleText.end(), datePat);
    boost::sregex_iterator mEnd;


    // TODO - get fiscal year end date??


    for(; mit != mEnd; ++mit)
    {
        //    for(size_t i = 0 ; i < mit->size() ; ++i)
            //   cout << "\n Match " << to_string(i) << " is : " << (*mit)[i].str() << endl;
        // returns the YEAR
        rInfo.push_back( (*mit)[3].str() );
    }

    return rInfo;
}

void
XmlElement::getNodes(string tagName, 
                     size_t number, 
                     vector<XmlElement*>* collected)
{
    if (collected->size() >= number )
        return;

    if (_tagName == tagName)
        collected->push_back(this);     

    for(auto it = _children.begin() ; it != _children.end() ; ++it)
        (*it)->getNodes(tagName, number, collected);
}


vector<string>
Parser::getRevenues(XmlElement* tree)
{
    string trTitle("Total revenue");
    return getTrByName(tree, trTitle);
}

vector<string> 
Parser::getIncs(XmlElement* tree){
    string trTitle("Net income");
    return getTrByName(tree, trTitle);
}

vector<float> 
Parser::getEps(XmlElement* tree){
    string trTitle("(diluted|dilution)");
    vector<float> retVec;
    vector<string> epsStrings = getTrByName(tree, trTitle);

    for(auto it = epsStrings.begin(); it != epsStrings.end(); ++it)
    {
        string clean = removeNonDigit( *it );
        retVec.push_back( stof(clean) );
    }
    return retVec;
}


vector<string> 
Parser::getTrByName(XmlElement* tree, string& trTitlePattern){

    string tagName("tr");
    XmlElement* dataLine = tree->tagWithText(tagName,trTitlePattern);
    string dataText = dataLine->text();

    cout << " \n Data line text is : " << dataText << endl;

    boost::regex pattern("(\\()?(\\d+)([,.]?\\d+)?(,?)(\\d+)?(\\))?");
    boost::sregex_iterator mit(dataText.begin(), dataText.end(), pattern);
    boost::sregex_iterator mEnd;

    vector<string> retVals;
    for(; mit != mEnd; ++mit)
    {
        string matchString = (*mit)[0].str();
        string cleanMatch = removeNonDigit( matchString );
        cout << "\n Adding extracted val : " << cleanMatch << endl;
        retVals.push_back( cleanMatch );
    }
    return retVals;
}


string 
Parser::extractLatest10kAcn(string& page)
{
    string startSearchS("<div id=\"seriesDiv");
    string openTag("<table");
    string closeTag("<\table");
    size_t startPos = page.find(startSearchS,0);
    startPos = page.find(openTag, startPos);
    size_t endPos = page.find(closeTag, startPos);
    string table = page.substr( startPos, (endPos-startPos) );

    cout << " Parsing out from string - \n" << table.substr(0,300) << endl;

    XmlElement* tree = buildXmlTree(table);
    string tagName("td");
    string ar("Annual report");
    XmlElement* tr = tree->tagWithText(tagName,ar);

    tr->printXmlTree(0);

    boost::regex pattern("(\\d+-\\d\\d-\\d+)");
    boost::smatch match;
    boost::regex_search(tr->text(), match, pattern);
//    cout << "\n Found text : " << tr->text() << endl;

    string acn = match[0]; 
    cout << "\n Found in it acn  : " << acn << endl;
   
    //
    return acn;
}
