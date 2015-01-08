#include <fstream>
#include <streambuf>
#include <iostream>
#include <map>
#include <boost/regex.hpp>
#include <math.h>  
#include "Utils.hpp"
#include "Logger.h"

#include "Tokenizer.h"
#include "Parser.h"

long
unitsToInt(string& units)
{
    units = toLower(units);
    if ( (units == "billions") ||
         (units == BILL) )
        return 1000000000;
    if ( (units == MILL) ||
         (units == "millions") )
        return 1000000;
    if ( (units == THOU) ||
         (units == "thousands") )
        return 1000;
    return 1;
}

size_t
countDecimals(string& num)
{
    size_t pos = num.find('.',0);
    if ( pos == string::npos )
        return 0;

    size_t num_dec(0);
    for(size_t i = pos + 1 ; i < num.size() ; ++i)
    {
        if ( ((num.at(i) - '0') > 9) || ((num.at(i) - '0') < 0) )
            break;
        ++num_dec;
    }
    return num_dec;
}
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
    boost::regex pattern( phrase, boost::regex::icase );

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
    LOG_INFO << "\n going to build xml tree";
    Tokenizer xmlTok = Tokenizer(xmlDocument);
    
    string rootName("__ROOT__");
    XmlElement* root = new XmlElement(rootName);

    parseXML( root, xmlTok);
    LOG_INFO << "Completed construction of tree";

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

    while ( ! tok.atEnd() )
    {
        xml_token = tok.xmlNextTok();
//        LOG_INFO << "\n procesing token: " << xml_token << "\n";

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

    //extract COVER report statement from dump file
    reportType = ReportType::COVER;
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
    //cout << "\n Called extract_quarterly_income() with downladed doc " 
    //     << page.substr(0,300) << endl;
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
        cout << "\n Found inc report " << reportKey << endl;
    }
    return retIncRep;
}


string 
Parser::extractFirstTableStr(string& incomeStr){
    string openTab("<table");
    string closeTab("</table");
    size_t startPos = incomeStr.find(openTab, 0);
    size_t endPos = incomeStr.find(closeTab, startPos);
    return incomeStr.substr( startPos , (endPos-startPos) );
}


vector<size_t> 
Parser::titleInfo(XmlElement* tree, string& units, string& currency,
    bool singleYear)
{
    string titleText = getUnitsAndCurrency( tree, units, currency);
//    LOG_INFO << "titleText is " << titleText;

    vector<size_t> years;    
    // get dates
    boost::regex datePat("(\\w\\w\\w). (\\d+)?, (\\d+)?");
    boost::sregex_iterator mit(titleText.begin(), titleText.end(), datePat);
    boost::sregex_iterator mEnd;

    // TODO - get fiscal year end date??

    for(; mit != mEnd; ++mit)
    {
        LOG_INFO << "Matches from title info for years are" 
                 << (*mit)[0].str() << " |  "
                 << (*mit)[1].str() << " |  "
                 << (*mit)[2].str() << " |  "
                 << (*mit)[3].str() << " |  ";
        string yearStr = (*mit)[3].str();
        LOG_INFO << "Adding year " << yearStr << " for data";
        years.push_back( stoi(yearStr) );
        if(singleYear)
            break;
    }
    return years;
}

string
get_units_from_text(string& text)
{
    boost::regex t_pattern ("In Thousands", boost::regex::icase);
    boost::regex m_pattern ("In Millions", boost::regex::icase);
    boost::regex b_pattern ("In Billions", boost::regex::icase);

    if (boost::regex_search(text, t_pattern) )
        return THOU;

    if (boost::regex_search(text, m_pattern) )
        return MILL;

    if (boost::regex_search(text, b_pattern) )
        return BILL;

    LOG_INFO << "Could not extract units from text: \n " << text;
    return "";
}

string
get_title_text(XmlElement* tree)
{
    vector<XmlElement*>* elements = new vector<XmlElement*>;
    string tagName("tr");
    tree->getNodes(tagName, 2, elements);
    string titleText  = (*elements)[0]->text() + (*elements)[1]->text() ;
    return titleText;
}

string
Parser::getUnitsAndCurrency(XmlElement* tree, string& units, string& currency)
{
    string titleText  = get_title_text(tree);
    LOG_INFO << "\n Title text is : " << titleText;

    // get currency
    boost::regex pattern("USD");
    currency = "USD";    
    if (!boost::regex_search(titleText, pattern) )
        currency = "OTHER";

    // get default units, make sure only first units appear in title text 
    size_t split = titleText.find("except");
    if (split != string::npos)
    {
        LOG_INFO << "Title text May contain multiple units. Spliting it";
        string preText = titleText.substr(0, split);
        units = get_units_from_text( preText );
    } else
        units = get_units_from_text( titleText );

    LOG_INFO << "(general) Units are being set to " << units 
             << " Currency is being set to " << currency ;

    return titleText;
}

void
Parser::parseQuarterlyIncomeStatment(XmlElement* tree, 
                                     string& units, string& currency,
                                     string& revenue, string& income, 
                                     float& eps, string& numshares)
{
    getUnitsAndCurrency( tree, units, currency );

    // get rev, inc, eps        
    revenue = removeNonDigit( getRevenues(tree,true).front() ) + units;
    income = removeNonDigit( getIncs(tree,true).front() ) + units;
    eps = getQarterEps(tree);
    numshares = getNumShares(tree, units)[0];
    cout << "\n Got income data: rev = " << revenue
         << "; inc = " << income << "; eps = " << to_string(eps) 
         << " numshares " << numshares << endl;
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
Parser::getRevenues(XmlElement* tree, bool singleYear)
{
    string trTitle("Total revenue");
    return getTrByName(tree, trTitle, singleYear);
}

vector<string> 
Parser::getIncs(XmlElement* tree, bool singleYear){
    string trTitle("Net income");
    return getTrByName(tree, trTitle, singleYear);
}

vector<string>
Parser::getNumShares(XmlElement* tree, string& bunits)
{
    vector<string> shares;
    string units("");

    //check if units for share number are specifide in table titles
    string titleText = get_title_text(tree);
    size_t split = titleText.find("except");
    if (split != string::npos)
    {
        string additional = titleText.substr(split, titleText.size()-split);
        LOG_INFO << "title text for table is " << titleText << 
            ". Which may contain additional info about units in the text: " 
                 <<  additional;

        boost::regex a_pattern ("share data", boost::regex::icase);
        if (boost::regex_search(additional, a_pattern) )
        {
            units = get_units_from_text( additional );
            if ( units != "")
            {   
                LOG_INFO << " Share data units found in title text of table."
                         << " units have been set to " << units; 
            }else{
                LOG_INFO << " Did not find addional units information in title text";
            }
            
        }
    }

    // find correct eps BLOCK in tree
    string tagName("table");
    XmlElement* tab = tree->firstNodebyName(tagName);
    Iterator iter(tab);
    XmlElement* tr;
    boost::regex u_pattern("(millions|thousands|billions)",
                           boost::regex::icase);
      
    // advance up to relavent block
    while ( (tr = iter.nextTr()) != NULL)
    {
        boost::regex block_pattern("(shares outstanding)", boost::regex::icase);
        boost::smatch match1;
        if ( boost::regex_search(tr->text(), match1, block_pattern) )
        {
            LOG_INFO << "\n Found Num shares block : \n" << tr->text();
            // check for units
            if ( boost::regex_search(tr->text(), match1, u_pattern) )
            {
                if (units != "")
                {
                    LOG_ERROR << "Found units both in title text, and in block"
                              << " title, using block info";
                }
                units =  match1[0].str();
                LOG_INFO <<"Units for numshares from block title are "<<units;
            }
            break;
        }
    }
    // find diluted - n
    while ( (tr = iter.nextTr()) != NULL)
    {
        boost::regex dil_pattern("(dilution|diluted)",boost::regex::icase);
        boost::smatch match2;
        string text = tr->text();
        if ( boost::regex_search(text, match2, dil_pattern) )
        {
            LOG_INFO << "\n Found num shares diluted  in : \n" << text;
            
            if ( boost::regex_search(text, match2, u_pattern) )
            {
                if (units != "")
                {
                    LOG_ERROR << "Found units both in data line, as well as"
                              << " previously. using data line units";
                }

                units =  match2[0].str();
                LOG_INFO << "numshare Units (from line) are " << units;
            }

            boost::regex dig_pat("[\\d,]+.?\\d+?");
            boost::sregex_iterator mit(text.begin(), text.end(), dig_pat);
            boost::sregex_iterator mEnd;
            for(; mit != mEnd; ++mit)
            {
                string matchString = (*mit)[0].str();
                size_t decimals = countDecimals( matchString );
                LOG_INFO << "\n Numshares extracted are: " << matchString << 
                    " wich has " << to_string(decimals) << " decimals";
                string cleanMatch = removeNonDigit( matchString );
                if ( units != "")
                {
                    LOG_INFO<<"\n using Units "<<units<<
                        " , wich differ from table units. Applying to "
                            <<cleanMatch;
                } else{
                    // If no units found
                    units = "1";
                    LOG_INFO << "No Units found for numshares. using units=1";
                }

                long ns = stol(cleanMatch) * unitsToInt( units);
                if( decimals > 0)
                    ns = ns / pow(10,decimals);
                cleanMatch = to_string( ns );
            
                LOG_INFO << "\n Adding extracted val : " << cleanMatch;
                shares.push_back( cleanMatch );
            }
        }
    }
    if ( shares.empty() )
    {
        LOG_INFO << " Could not read number of shares outstanding from table";
        // Where to get data from????

    }
    return shares;
}


string 
Parser::getNumSharesFromCoverReport(string& report)
{
    string tableStr = extractFirstTableStr(report); 
    XmlElement* tree = buildXmlTree(tableStr);
    string trTitle("Entity Common Stock, Shares Outstanding");
    string tagName("tr");
    XmlElement* dataLine = tree->tagWithText(tagName,trTitle);
    string dataText = dataLine->text();
    LOG_INFO << " Text containing number of shares is " << dataText;
    
    boost::regex pattern("\\d+[,\\d]+\\d+");
    boost::smatch match;
    string numshares("");
    if (boost::regex_search(dataText, match, pattern) )
    {
        numshares = match[0];
        LOG_INFO << "Got num shares from cover report. They are: "<< numshares;
    } else
        LOG_ERROR << "Could not get numshares from cover report";
    
    numshares = removeNonDigit( numshares );
    return numshares;
}


vector<float> 
Parser::getAnualEps(XmlElement* tree, bool singleYear){
    string trTitle("(diluted|dilution)");
    string tagName("tr");

    XmlElement* dataLine = tree->tagWithText(tagName,trTitle);

    if ( dataLine == NULL )
        LOG_ERROR << "Could not extract " << tagName 
                  << " with text matching pattern " << trTitle;

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
        if (singleYear)
            break;
    }
    return retVals;
}

double
Parser::getQarterEps(XmlElement* tree)
{
    double retEps(0);
    bool foundEps(false);
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
            LOG_INFO << "\n Found eps block : \n" << tr->text();
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
            LOG_INFO << "\n Found eps in : \n" << tr->text();
            boost::regex dig_pat("\\d+.(\\d+)?");
            boost::smatch match3;
            boost::regex_search(tr->text(), match3, dig_pat);
            retEps = stof( match3[0] );
            foundEps = true;
            break;
        }
    }
    if (foundEps)
        return retEps;

    // The above code faild to retrieve eps data
    // we will start heuristic number 2:
    LOG_INFO << "Failed to find eps so far. Going to use heuristic #2";
    Iterator iterb(tab);

    // advance up to relavent block
    while ( (tr = iterb.nextTr()) != NULL)
    {
        boost::regex block_pattern("(per share of common stock)",
                                   boost::regex::icase);
        boost::smatch match1;
        if ( boost::regex_search(tr->text(), match1, block_pattern) )
        {
            LOG_INFO << "\n (H2) Found eps block : \n" << tr->text();
            break;
        }
    }

    boost::regex h2_pattern("diluted",boost::regex::icase);
    while ( (tr = iterb.nextTr()) != NULL)
    {
        boost::smatch match4;
        if ( boost::regex_search(tr->text(), match4, h2_pattern) )
        {
            LOG_INFO << "\n Found eps in tr : \n" << tr->text();
            boost::regex dig_pat("\\d+.(\\d+)?");
            boost::smatch match3;
            boost::regex_search(tr->text(), match3, dig_pat);
            retEps = stof( match3[0] );
            foundEps = true;
            break;
        }
    }
    LOG_INFO << "Returning eps value" << to_string(retEps);
    return retEps;
}

vector<string> 
Parser::getTrByName(XmlElement* tree, string& trTitlePattern, bool singleYear){

    string tagName("tr");
    XmlElement* dataLine = tree->tagWithText(tagName,trTitlePattern);

    if ( dataLine == NULL )
        LOG_ERROR << "Could not extract " << tagName 
                  << " with text matching pattern " << trTitlePattern;

    string dataText = dataLine->text();
    LOG_INFO << " \n Data line text is : " << dataText;

    boost::regex pattern("(\\()?(\\d+)([,.]?\\d+)?(,?)(\\d+)?(\\))?");
    boost::sregex_iterator mit(dataText.begin(), dataText.end(), pattern);
    boost::sregex_iterator mEnd;

    vector<string> retVals;
    for(; mit != mEnd; ++mit)
    {
        string matchString = (*mit)[0].str();
        string cleanMatch = removeNonDigit( matchString );
        LOG_INFO << "\n Adding extracted val : " << cleanMatch;
        retVals.push_back( cleanMatch );
        if (singleYear)
            break;
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
        LOG_INFO << "\n Could not extract acn from tr with text: "<< text;
        return NULL;
    }

    string acn = match1[0];

    boost::regex date_pattern("(\\s\\d\\d\\d\\d-\\d\\d-\\d\\d\\s)");
    boost::smatch match;
    boost::regex_search(text, match, date_pattern);

    date report_date( from_string( match[0] ) );

    Acn* acn_rec = new Acn( acn, report_date, 1);
    acn_rec->set_quarter_from_date();

    LOG_INFO << "Extracted ACN: "<< acn << " filed on date" << report_date;
    return acn_rec;
}

vector<Acn*> 
Parser::getAcnsFromSearchResults(string& page, 
                                 size_t limit,/*limit==0 limits to last year*/ 
                                 StatementType st)
{
    vector<Acn*> acns;

    date today = day_clock::local_day();
    greg_year last_year = today.year() - 1;

    XmlElement* table = edgarResultsTableToTree( page );
    string tagName("table");
    table = table->firstNodebyName( tagName );

    size_t counter = 0;
    for(auto it = table->_children.begin() ; it != table->_children.end(); ++it)
        if ( (*it)->_tagName == "tr" )
        {
            Acn* acn = trToAcn( *it );

            if (acn == NULL)
                continue;

            if ( (limit!=0) && (counter >= limit) )
                break;

            if ( (limit==0) && (acn->_report_date.year() < last_year) ) 
                break;

            if (st == StatementType::K10)
                acn->_quarter = 0;

            acns.push_back( acn );
            counter++;
        }
    return acns;
}


