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

    for(size_t i = 0; i < depth+1; ++i)
        cout << "  ";

*/
    if ( _text == "" )
        cout << " \t Contains no text" << endl;
    else
        cout << " \t Content is: " << _text << endl;

    for( auto it = _children.begin(); it != _children.end(); ++it )
        (*it)->printXmlTree( depth+1);
}

XmlElement*
Iterator::nextTr()
{
    size_t num_children = _node->_children.size();
    if ( _i >= num_children )
        return NULL;

    while ( _node->_children[_i]->_tagName != "tr")
    {
        _i = _i + 1;
        if ( _i >= num_children )
            return NULL;
    }
    _i = _i + 1;
    return _node->_children[_i-1];
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
XmlElement::firstNodebyName(string& tagName ){
    if ( _tagName == tagName )
        return this;
   
    XmlElement* node = NULL;
    for( auto it = _children.begin(); it != _children.end(); ++it )
    {
        node = (*it)->firstNodebyName( tagName );
        if (node != NULL)
            return node;
    }
    return NULL;
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

    if ( ( xml == "<br>")       ||
         ( xml == "<br />")     ||
         ( xml.substr(0,4) == "<!--")    ||
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
        //cout << "\n procesing token: " << xml_token << endl;

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
            //cout << "<- IGNORE token" << endl;
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
Parser::extract_quarterly_income(string& page)
{
    Tokenizer tokenizer(page);
    string filingSummary = tokenizer.findFilingSummary();

    Tokenizer filingSummaryTok(filingSummary);
    auto reports = new map<ReportType,string>;
    filingSummaryTok.getReportDocNames(reports);

    //extract INCOME statement from dump file
    string retIncRep("");
    string reportKey;
    ReportType reportType = ReportType::INCOME;
    if ( (reports->find(reportType)) != reports->end() )
    {   
        reportKey = reports->find(reportType)->second;
        string docString = "<FILENAME>"+reportKey;
        retIncRep = tokenizer.findDoc(docString);
    }
    return retIncRep;
}


string 
Parser::extractIncomeTableStr(string& incomeStr){
    string openTab("<table");
    string closeTab("</table");
    size_t startPos = incomeStr.find(openTab, 0);
    size_t endPos = incomeStr.find(closeTab, startPos);
    return incomeStr.substr( startPos , (endPos-startPos) );
}


vector<size_t> 
Parser::titleInfo(XmlElement* tree, string& units, string& currency)
{
    string titleText = getUnitsAndCurrency( tree, units, currency );

    vector<size_t> years;    
    // get dates
    boost::regex datePat("(\\w\\w\\w). (\\d+)?, (\\d+)?");
    boost::sregex_iterator mit(titleText.begin(), titleText.end(), datePat);
    boost::sregex_iterator mEnd;

    // TODO - get fiscal year end date??

    for(; mit != mEnd; ++mit)
    {
        //    for(size_t i = 0 ; i < mit->size() ; ++i)
            //   cout << "\n Match " << to_string(i) << " is : " << (*mit)[i].str() << endl;
        string yearStr = (*mit)[3].str();
        years.push_back( stoi(yearStr) );
    }
    return years;
}

string
Parser::getUnitsAndCurrency(XmlElement* tree, 
                            string& units, string& currency)
{
    vector<XmlElement*>* elements = new vector<XmlElement*>;
    string tagName("tr");
    tree->getNodes(tagName, 2, elements);
 
    string titleText  = (*elements)[0]->text() + (*elements)[1]->text() ;
    
    //cout << "\n Title text is : " << titleText << endl; 
    
    // get units
    boost::regex pattern ("In Thousands");
    if (boost::regex_search(titleText, pattern) )
        units = THOU;
    pattern = "In Millions";
    if (boost::regex_search(titleText, pattern) )
        units = MILL;
    pattern = "In Billions";
    if (boost::regex_search(titleText, pattern) )
        units = BILL;

    // get currency
    pattern = "USD";
    if (!boost::regex_search(titleText, pattern) )
        currency = "OTHER";
    currency = "USD";    

    return titleText;
}

void
Parser::parseQuarterlyIncomeStatment(XmlElement* tree, 
                                     string& units, string& currency,
                                     string& revenue, string& income, 
                                     double& eps)
{
    getUnitsAndCurrency( tree, units, currency );

    // get rev, inc, eps        
    revenue = removeNonDigit( getRevenues(tree).front() ) + units;
    income = removeNonDigit( getIncs(tree).front() ) + units;
    eps = getQarterEps(tree);
    
    cout << "\n Got income data: rev = " << revenue
         << "; inc = " << income << "; eps = " << to_string(eps) << endl;
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

vector<string>
Parser::getNumShares(XmlElement* tree, string& bunits)
{
    cout << "\n Going to retrieve num shares" << endl;
    vector<string> shares;
    string units("");

    // find correct eps BLOCK in tree
    string tagName("table");
    XmlElement* tab = tree->firstNodebyName(tagName);
    Iterator iter(tab);
    XmlElement* tr;

    boost::regex u_pattern("\\(in (\\w+)\\)|(millions)\\)", boost::regex::icase);
      
    // advance up to relavent block
    while ( (tr = iter.nextTr()) != NULL)
    {
        boost::regex block_pattern("(shares outstanding)", boost::regex::icase);
        boost::smatch match1;
        if ( boost::regex_search(tr->text(), match1, block_pattern) )
        {
            cout << "\n Found Num shares block : \n" << tr->text() << endl;
            // check for units
            if ( boost::regex_search(tr->text(), match1, u_pattern) )
            {
                units =  match1[1].str();
                cout << "Units are " << units << endl;
            }
            break;
        }
    }
    // find diluted - n
    while ( (tr = iter.nextTr()) != NULL)
    {
        boost::regex dil_pattern("(dilution)");
        boost::smatch match2;
        string text = tr->text();
        if ( boost::regex_search(text, match2, dil_pattern) )
        {
            cout << "\n Found num shares diluted  in : \n" << text << endl;

            if ( boost::regex_search(text, match2, u_pattern) )
            {
                units =  match2[1].str();
                cout << "Units (from line) are " << units << endl;
            }

            boost::regex dig_pat("[\\d,]+.?\\d+?");
            boost::sregex_iterator mit(text.begin(), text.end(), dig_pat);
            boost::sregex_iterator mEnd;
            for(; mit != mEnd; ++mit)
            {
                string matchString = (*mit)[0].str();
                string cleanMatch = removeNonDigit( matchString );
                if ( units != "")
                    //stod(matchString) * unitsToInt( units);
                    ;
                cout << "\n Adding extracted val : " << cleanMatch << endl;
                shares.push_back( cleanMatch );
            }
    
        }
    }
    return shares;
}


vector<float> 
Parser::getAnualEps(XmlElement* tree){
    string trTitle("(diluted|dilution)");
    string tagName("tr");

    XmlElement* dataLine = tree->tagWithText(tagName,trTitle);
    string dataText = dataLine->text();

//    cout << " \n Data line text is : " << dataText << endl;

    boost::regex pattern("\\(?(\\d+)(.\\d+)?\\)?");
    boost::sregex_iterator mit(dataText.begin(), dataText.end(), pattern);
    boost::sregex_iterator mEnd;

    vector<float> retVals;
    for(; mit != mEnd; ++mit)
    {
        string val = (*mit)[0].str();
        //      cout << "\n Adding extracted val : " << val << endl;
        retVals.push_back( stof(val) );
    }
    return retVals;
}

double
Parser::getQarterEps(XmlElement* tree)
{
    cout << "\n Going to retrieve diluted eps for quarter" << endl;
    double retEps(0);
    // find correct eps BLOCK in tree
  
    string tagName("table");
    XmlElement* tab = tree->firstNodebyName(tagName);
    Iterator iter(tab);
    XmlElement* tr;
    // advance up to relavent block
    while ( (tr = iter.nextTr()) != NULL)
    {
        boost::regex block_pattern("(Assuming dilution|Earnings per share of common stock)");
        boost::smatch match1;
        if ( boost::regex_search(tr->text(), match1, block_pattern) )
        {
            //cout << "\n Found block : \n" << tr->text() << endl;
            break;
        }
    }
    // find diluted - n
    while ( (tr = iter.nextTr()) != NULL)
    {
        boost::regex eps_pattern("(Total \\(in dollars per share\\)|dilution)");
        boost::smatch match2;
        if ( boost::regex_search(tr->text(), match2, eps_pattern) )
        {
//            cout << "\n Found eps in : \n" << tr->text() << endl;
            boost::regex dig_pat("\\d+.(\\d+)?");
            boost::smatch match3;
            boost::regex_search(tr->text(), match3, dig_pat);
            retEps = stof( match3[0] );
            break;
        }
    }
    return retEps;
}

vector<string> 
Parser::getTrByName(XmlElement* tree, string& trTitlePattern){

    string tagName("tr");
    XmlElement* dataLine = tree->tagWithText(tagName,trTitlePattern);
    string dataText = dataLine->text();

    //cout << " \n Data line text is : " << dataText << endl;

    boost::regex pattern("(\\()?(\\d+)([,.]?\\d+)?(,?)(\\d+)?(\\))?");
    boost::sregex_iterator mit(dataText.begin(), dataText.end(), pattern);
    boost::sregex_iterator mEnd;

    vector<string> retVals;
    for(; mit != mEnd; ++mit)
    {
        string matchString = (*mit)[0].str();
        string cleanMatch = removeNonDigit( matchString );
        //cout << "\n Adding extracted val : " << cleanMatch << endl;
        retVals.push_back( cleanMatch );
    }
    return retVals;
}

XmlElement*
Parser::edgarResultsTableToTree(string& page)
{
    string startSearchS("<div id=\"seriesDiv");

    string openTag("<table");
    string closeTag("</table");
    size_t startPos = page.find(startSearchS,0);
    startPos = page.find(openTag, startPos);
    size_t endPos = page.find(closeTag, startPos);
    string table = page.substr( startPos, (endPos-startPos) );

    XmlElement* tree = buildXmlTree(table);
    return tree;
}


string 
Parser::extractLatest10kAcn(string& page)
{
    XmlElement* tree = edgarResultsTableToTree( page );

    string tagName("td");
    string ar("Annual report");
    XmlElement* tr = tree->tagWithText(tagName,ar);

    tr->printXmlTree(0);

    boost::regex pattern("(\\d+-\\d\\d-\\d+)");
    boost::smatch match;
    boost::regex_search(tr->text(), match, pattern);
//    cout << "\n Found text : " << tr->text() << endl;

    string acn = match[0]; 
//    cout << "\n Found in it acn  : " << acn << endl;
    return acn;
}

// return NULL on failure
Acn*
Parser::trToAcn( XmlElement* tr )
{
    if ( tr->_tagName != "tr" )
        return NULL;    

    string text = tr->text();

    boost::regex acn_pattern("(\\d+-\\d\\d-\\d+)");
    boost::smatch match1;
    if (! boost::regex_search(text, match1, acn_pattern) )
    {
        cout << "\n Could not extract acn from tr with text: " << text << endl;
        return NULL;
    }

    string acn = match1[0];

    boost::regex date_pattern("(\\s\\d\\d\\d\\d-\\d\\d-\\d\\d\\s)");
    boost::smatch match;
    boost::regex_search(text, match, date_pattern);

    date report_date( from_string( match[0] ) );

    Acn* acn_rec = new Acn( acn, report_date, 1);
    acn_rec->set_quarter_from_date();

    return acn_rec;
}

vector<Acn*> 
Parser::getQuarterAcns(string& page)
{
    vector<Acn*> acns;
    date today = day_clock::local_day();
    greg_year last_year = today.year() - 1;

    XmlElement* table = edgarResultsTableToTree( page );

    string tagName("table");
    table = table->firstNodebyName( tagName );

    for(auto it = table->_children.begin() ; it != table->_children.end(); ++it)
        if ( (*it)->_tagName == "tr" )
        {
            Acn* acn = trToAcn( *it );
            if (acn == NULL)
                continue;
            if ( acn->_report_date.year() < last_year )
                break;
            acns.push_back( acn );
        }
    return acns;
}


