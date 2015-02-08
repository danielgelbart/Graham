#include <fstream>
#include <streambuf>
#include <iostream>
#include <map>
#include <math.h>  
#include "Utils.hpp"
#include "Logger.h"
#include "Tokenizer.h"
#include "Parser.h"

using namespace DMMM;
using namespace boost;

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

bool
containsNoData(string& dataText )
{
    boost::regex pattern("&#xA0;");

    if ( boost::regex_search(dataText, pattern) )
        return true;
    return false;
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
trIterator::nextTr()
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
XmlElement*
tdIterator::at(size_t i)
{
//    LOG_INFO << "called at() with value"<< to_string(i);
    if (_node == NULL)
        LOG_ERROR << "node is NULL when calling at()";

    size_t num_children = _node->_children.size();
    LOG_INFO << "Node has "<<to_string(num_children) << " children";
    if ( i >= num_children )
        return NULL;
//    LOG_INFO << "Going to return "<< _node->text() << "   child number"<<to_string(i);
    return _node->_children[i];
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
XmlElement::tagWithText(string& tagName, string& phrase, 
                        const size_t num, size_t* counter)
{
    boost::regex pattern( phrase, boost::regex::icase );
    LOG_INFO << "comparing to tag with text: "<< text()<<"\n";

    if (_tagName == tagName)
    {
        if ( boost::regex_search( text(), pattern ) )
        {
            LOG_INFO<<"counter is "<< to_string(*counter)<<"\n";
            *counter =  *counter+1;
            if (*counter == num)
                return this;
            else
                return NULL;
        }
    }
    if ( _children.empty() )
        return NULL;
    
    XmlElement* node = NULL;
    for( auto it = _children.begin(); it != _children.end(); ++it )
    {
        node = (*it)->tagWithText( tagName, phrase, num, counter );
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

string
get_title_text(XmlElement* tree)
{
    vector<XmlElement*>* elements = new vector<XmlElement*>;
    string tagName("tr");
    tree->getNodes(tagName, 2, elements);
    string titleText  = (*elements)[0]->text() + (*elements)[1]->text() ;
    return titleText;
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
    vector<string> retVals;
    string trTitle("Total revenue");
    retVals = getTrByName(tree, trTitle, singleYear);
    if (retVals.empty())
    {
        trTitle = "Net revenue";
        retVals = getTrByName(tree, trTitle, singleYear);
    }
    if (retVals.empty())
    {
        trTitle = "Operating revenues?";
        retVals = getTrByName(tree, trTitle, singleYear);
    }
    if (retVals.empty())
    {
        // Added for BDX
        trTitle = "^((\\s|:)*Revenues)";
        retVals = getTrByName(tree, trTitle, singleYear);
    }
    return retVals;
}

vector<string> 
Parser::getIncs(XmlElement* tree, bool singleYear){
    // T, F report income this way:
    string trTitle("^((\\s|:)*Net income attributable to)");
    vector<string> retVals;
    retVals = getTrByName(tree, trTitle, singleYear);
    if (retVals.empty())
    {
        // Normal companies use this title:
        trTitle = "^((\\s|:)*Net income)";
        retVals = getTrByName(tree, trTitle, singleYear);
    }
    return retVals;
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
    trIterator iter(tree);
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
                if( ( ns < 1000000 ) && (1==unitsToInt(units)) )
                {
                    LOG_INFO<<"\n**NOTE: Retrieved numshares value of: "
                            << to_string(ns)<<" wich seems low, as well as not"
                            << " being able to extract units for numshares. "
                            << "Therefore, using units found for general porpus"
                            << "e " << bunits<<" \n";
                    ns = ns*unitsToInt( bunits );
                }
                cleanMatch = to_string( ns );
            
                LOG_INFO << "\n Adding extracted val : " << cleanMatch;
                shares.push_back( cleanMatch );
            }
        }
    }
    if ( shares.empty() )
        LOG_INFO << " Could not read number of shares outstanding from table";
    return shares;
}

string 
Parser::getNumSharesFromCoverReport(string& report)
{
    if (report =="")
    {
            LOG_ERROR << "No cover report available. So exusted attempt to "
                      << "read num shares";
            return "";

    }
    string tableStr = extractFirstTableStr(report); 
    XmlElement* tree = buildXmlTree(tableStr);
    string units="";

    //check if units for shares appear in title
    string titleText = get_title_text(tree);
    size_t split = titleText.find("except");
    if (split != string::npos)
    {
        string additional = titleText.substr(split, titleText.size()-split);
        LOG_INFO << "Found possible additional units in title text: "
                 << titleText;

        boost::regex a_pattern ("share data", boost::regex::icase);
        if (boost::regex_search(additional, a_pattern) )
            units = get_units_from_text( additional );
    }
    string trTitle("Entity Common Stock, Shares Outstanding");
    string tagName("tr");

    size_t* counter = new size_t;
    *counter=0;
    XmlElement* dataLine = tree->tagWithText(tagName,trTitle,1,counter);
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
    if ((units != "") && (units != "1"))
    { 
        LOG_INFO << " Share data units found in title text of table."
               << " units have been set to " << units; 
        numshares = (numshares+units);
    }
    return numshares;
}

void
writeRevenueToEarnings(O_Ep& ep, string& val, string& units)
{
    ep._revenue() = removeNonDigit(val)+units;
}
void
writeIncomeToEarnings(O_Ep& ep, string& val, string& units)
{
    ep._net_income() = removeNonDigit(val)+units;
}
void
writeEpsToEarnings(O_Ep& ep, string& val, string& units)
{
    LOG_INFO << "Writing eps of "<<val<<"\n";
    ep._eps() = stod( val );
}
void
writeNsToEarnings(O_Ep& ep, string& val, string& units)
{
    // pass both units+bunits as "units
    // so split
    size_t sPos = units.find("|",0);
    sPos += 1; // to point past the '|'
    string bunits = units.substr(sPos, (units.length() - sPos));
    units = units.substr(0,sPos-1);
    LOG_INFO << "units for shares are: "<< units<<" general units are"<<bunits;
    string matchString = val;
    size_t decimals = countDecimals( matchString );
    string cleanMatch = removeNonDigit( matchString );

    LOG_INFO << "\n Clean match for value with dec point is "<<cleanMatch
             << "which has "<<to_string(decimals) << " decimals";

    if ( units == "")
        units = "1";

    long ns = stol(cleanMatch) * unitsToInt( units);

    if( decimals > 0)
        ns = ns / pow(10,decimals);

    // Handel errouneous units case
    if( ( ns < 1000000 ) && (1==unitsToInt(units)) )
    {
        LOG_INFO<<"\n**NOTE: Retrieved numshares value of: "
                << to_string(ns)<<" wich seems low, as well as not"
                << " being able to extract units for numshares. "
                << "Therefore, using units found for general porpus"
                << "e " << bunits<<" \n";
        units = bunits;
        ns = ns*unitsToInt( units );

        // add decimal values if exist
        if( decimals > 0)
        {

            string dec_mathces =
                cleanMatch.substr( (cleanMatch.length()-decimals), decimals); 
            ns +=  stol(dec_mathces) * ( unitsToInt( units) / pow(10,decimals));
        }
    }
    cleanMatch = to_string( ns );
    LOG_INFO <<"Adding Num Shares value of: "<< cleanMatch<<"\n";
    ep._shares() =  cleanMatch ;
}


bool 
testBlock(string& text, regex& block_pattern)
{
//    regex eps_pattern("Earnings per share", regex::icase);
    if ( (regex_search(text, block_pattern)) &&
         (containsNoData( text )) )
        return true;
    return false;
}

bool
Parser::checkTrPattern( string& text, boost::regex& title_pattern, 
                        string& units, XmlElement* node,
                        boost::regex& extract_pattern, O_Ep& earnings,
                        void(*func)(O_Ep&,string&,string&))
{
    boost::smatch match;
    if ( boost::regex_search( text, title_pattern ) )
    {
        LOG_INFO << "\n regex match in text: "<<text<<"\n col to get is: "
                 <<to_string(_col_num);
        
        // get i'th td from tr
        tdIterator tdIt(node);
        XmlElement* node = tdIt.at(_col_num);
        if (node == NULL)
            cout << "td is NULL";
        string tdtext = node->text();

        LOG_INFO << "Matching val from text: "<<tdtext;
        if (boost::regex_search(tdtext, match, extract_pattern) )
        {       
            string val = match[0];
            LOG_INFO << "\n extracted val is"<< val;
            func(earnings,val,units);
            return true;
        }
    }
    return false;
} 

string
checkForShareUnits(const string& text)
{
    regex u_pattern("(millions|thousands|billions|in shares)", regex::icase);
    boost::smatch match;
    string units = "";
    if ( boost::regex_search(text, match, u_pattern) )
        units =  match[0].str();
    return units;
}

string
checkForShareUnitsInTitle(const string& titleText)
{
    string units = "";
    size_t split = titleText.find("except");
    if (split != string::npos)
    {
        string additional = titleText.substr(split, titleText.size()-split);
        LOG_INFO << "Checking for share units in title text ";
        boost::regex a_pattern ("share data", boost::regex::icase);
        if (boost::regex_search(additional, a_pattern) )
            units = get_units_from_text( additional );
    }
    return units;
}

size_t 
Parser::findColumnToExtract(XmlElement* tree, DMMM::O_Ep& earnigs_data)
{
    string titleText = get_title_text(tree);
    regex date_pattern("(\\w\\w\\w). (\\d+)?, (\\d+)?");

// calculate relavent end date from - stock.fye, ep.year, ep.quarter
    date end_date = calculateEndDate(_stock._fiscal_year_end(),
                                     earnigs_data._year(),
                                     earnigs_data._quarter());
// iterate over dates in text
    // if found match
    // save column num
    // if next column date exactly 1 year before, exit 
    // else continue
    sregex_iterator mit(titleText.begin(), titleText.end(), date_pattern);
    sregex_iterator mEnd;
    size_t col_num = 1;
    bool foundDate = false;
    for(size_t i = 0; mit != mEnd; ++mit)
    {
        ++i;
        string dateStr = (*mit)[0].str();
        date rep_date = convertFromDocString(dateStr);
        cout << "\n Examining date: "<< to_simple_string(rep_date)<<endl;
        if (foundDate)
        {
            if ( ( rep_date == (end_date - years(1)) ) &&
                 ( col_num == i-1 ) )
            {
                LOG_INFO<<"Setting column index to "<<to_string(col_num);
                break;
            }
        }
        if (rep_date == end_date)
        {
            cout << "^ This date matches the requested report date."<<endl;
            col_num = i;
            foundDate = true;
        }
    }
    return col_num;
}

bool
Parser::extractTotalRevenue(XmlElement* tree, DMMM::O_Ep& earnigs_data,
                    string& units)
{
    trIterator trIt(tree);
    XmlElement* trp = tree;

    bool foundRev(false);
    bool foundRevBlock(false);

    regex num_pattern("\\d+[,\\d]+\\d+(.\\d+)?");
    regex eps_pattern("(diluted|dilution)", regex::icase);
    regex date_pattern("(\\w\\w\\w). (\\d+)?, (\\d+)?");

    smatch match;

    // first, try to get single line in one shot
    while( (trp = trIt.nextTr()) != NULL )
    {
        string trtext = trp->text();
        LOG_INFO << "\n Handling line - \n" << trtext;

        boost::regex rev_pattern("Total revenue", boost::regex::icase );
        if ( regex_search( trtext, rev_pattern)) {
            foundRev = checkTrPattern( trtext, rev_pattern, units, trp,
                         num_pattern, earnigs_data, writeRevenueToEarnings);
            break;
        }
            
        rev_pattern.assign("Net revenue",boost::regex::icase);
        if (regex_search( trtext, rev_pattern)){
            foundRev = checkTrPattern( trtext, rev_pattern, units, trp,
                         num_pattern, earnigs_data, writeRevenueToEarnings);
            break;
        }
        rev_pattern.assign("Operating revenues?",boost::regex::icase);
        if (regex_search( trtext, rev_pattern)){
            foundRev = checkTrPattern( trtext, rev_pattern, units, trp,
                                       num_pattern, earnigs_data, writeRevenueToEarnings);
            break;
        }
        // Added for BDX/MSFT
        rev_pattern.assign("^((\\s|:)*Revenues?)",boost::regex::icase);
        if (regex_search( trtext, rev_pattern)){
            foundRev = checkTrPattern( trtext, rev_pattern, units, trp,
                                       num_pattern, earnigs_data, writeRevenueToEarnings);
            break;
        }
    } // while loop over table

    // if not found - iterate in block search mode
    trIt.resetToStart();
    if(!foundRev){
        LOG_INFO << "Looking for revenue by block strocture";
        while( (trp = trIt.nextTr()) != NULL )
        {
            string trtext = trp->text();
            if(!foundRevBlock)
            {
                // DE Block
                regex block_pattern("Sales and Revenues",regex::icase);
                if((foundRevBlock = testBlock(trtext,block_pattern)))
                    continue;
                // CVX block
                block_pattern.assign("^((\\s|:)*Revenues?)",regex::icase);
                if((foundRevBlock = testBlock(trtext,block_pattern)))
                    continue;
            } else {
                // DE Pattern
                regex rev_pattern( "Total", boost::regex::icase );
                if ((foundRev = checkTrPattern( trtext, rev_pattern, units, trp,
                        num_pattern, earnigs_data, writeRevenueToEarnings)))
                    break;
            }
        } // while for block find
    } // if !foundRev
    return foundRev;
}

bool
Parser::extractNetIncome(XmlElement* tree, DMMM::O_Ep& earnings_data,
                            string& units)
{
    trIterator trIt(tree);
    XmlElement* trp = tree;
    bool foundInc(false);
    regex num_pattern("\\d+[,\\d]+\\d+(.\\d+)?");

    while( (trp = trIt.nextTr()) != NULL )
    {
        string trtext = trp->text();

        // T, F report income this way:
        regex inc_pattern("^((\\s|:)*Net income attributable to)",
                          regex::icase);
        if ((foundInc = checkTrPattern( trtext, inc_pattern, units, trp,
                       num_pattern, earnings_data, writeIncomeToEarnings)))
                break;
        // Normal companies use this title:
        inc_pattern.assign("^((\\s|:)*Net income)",boost::regex::icase);
        if ((foundInc = checkTrPattern( trtext, inc_pattern, units, trp,
                       num_pattern, earnings_data, writeIncomeToEarnings)))
                continue;
    } // while
    return foundInc;
}


bool
Parser::extractEps(XmlElement* tree, DMMM::O_Ep& earnings_data,string& units)
{
    trIterator trIt(tree);
    XmlElement* trp = tree;

    bool foundEps(false);
    bool foundEpsBlock(false);

    regex num_pattern("\\d+[,\\d]+\\d+(.\\d+)?");

    regex digit_pattern("\\(?(\\d+)(.\\d+)?\\)?");
    regex date_pattern("(\\w\\w\\w). (\\d+)?, (\\d+)?");

    smatch match;

    // First, search for block structure
    while( (trp = trIt.nextTr()) != NULL )
    {
        string trtext = trp->text();

        // find Eps block
        if(!foundEps && !foundEpsBlock)
        {
            regex eblock_pattern("Earnings per share",regex::icase);
            if((foundEpsBlock = testBlock(trtext,eblock_pattern)))
                continue;
            eblock_pattern.assign("per share",regex::icase);
            if((foundEpsBlock = testBlock(trtext,eblock_pattern)))
                continue;
        }
        // find eps IN block
        if(foundEpsBlock && !foundEps)
        {
            regex eps_pattern("(diluted|dilution)", regex::icase);
            if((foundEps = checkTrPattern(trtext, eps_pattern, units, trp,
                                          digit_pattern, earnings_data, writeEpsToEarnings)))
                break;
        } // end find diluted eps
    } // while

    // if not found - iterate again without block search mode
    if(!foundEps){
        trIt.resetToStart();
        LOG_INFO << "Looking for eps with no block strocture";
        while( (trp = trIt.nextTr()) != NULL )
        {
            string trtext = trp->text();
            // for INTC
            regex eps_pattern("diluted (earnings|income)", regex::icase);
            if((foundEps = checkTrPattern(trtext, eps_pattern, units, trp,
                             digit_pattern, earnings_data, writeEpsToEarnings)))
                continue;
            // For GOOG
            eps_pattern.assign("per share ([\\w\\s-]+) diluted",
                                  regex::icase);
            if((foundEps = checkTrPattern(trtext, eps_pattern, units, trp,
                          digit_pattern, earnings_data, writeEpsToEarnings)))
                break;

        } // while
    } // if not found Eps in block structure
    return foundEps;    
}

bool
Parser::extractNumShares(XmlElement* tree, DMMM::O_Ep& earnings_data,
                         string& units, string& nsrUnits)
{
    trIterator trIt(tree);
    XmlElement* trp = tree;

    bool foundNsrBlock(false);
    bool foundNsr(false);

    regex num_pattern("\\d+[,\\d]+\\d+(.\\d+)?");
    regex nsunits_pattern("(millions|thousands|billions)",regex::icase);
    regex date_pattern("(\\w\\w\\w). (\\d+)?, (\\d+)?");

    while( (trp = trIt.nextTr()) != NULL )
    {
        string trtext = trp->text();

        if(!foundNsrBlock)
        {
            regex sblock_pattern("shares outstanding",regex::icase);
            if((foundNsrBlock = testBlock(trtext,sblock_pattern)))
            {
                if (nsrUnits != "")
                    nsrUnits = checkForShareUnits(trtext);
                continue;
            }
        } 
        if(foundNsrBlock && !foundNsr)
        {
            regex ds_pattern("(dilution|diluted)",regex::icase);
            if ( regex_search( trtext, ds_pattern ) )
            {
                // check for units in trtext
                if(nsrUnits != "")
                    nsrUnits = checkForShareUnits(trtext);
 
                nsrUnits += "|"+units;
                foundNsr = checkTrPattern(trtext, ds_pattern, nsrUnits, trp,
                                          num_pattern, earnings_data, writeNsToEarnings);
                continue;
            }
        } // find line
    } // while
    return foundNsr;
}

void 
Parser::parseIncomeTree(XmlElement* tree, DMMM::O_Ep& earnings_data)
{
    trIterator trIt(tree);
//    tree->printXmlTree(0);

    bool foundRev(false);
    bool foundInc(false);
    bool foundEps(false);
    bool foundNsr(false);

    string nsrUnits = "";
    string units = "";
    string currency = "";
    regex num_pattern("\\d+[,\\d]+\\d+(.\\d+)?");
//    regex num_pattern("\\d+[,\\d]+\\d+");
//    regex eps_pattern("(diluted|dilution)", regex::icase);
    regex nsunits_pattern("(millions|thousands|billions)",regex::icase);
    regex date_pattern("(\\w\\w\\w). (\\d+)?, (\\d+)?");

    // get units from title
    string titleText = getUnitsAndCurrency( tree, units, currency);
    if(titleText == "")
    {
        LOG_ERROR<<"Malformed income table, or at least title is malformed";
        return;
    }
    smatch match;
    regex_search(titleText, match, date_pattern);
//    cout << "\n Matches are 0: "<<match[0]<<" 1: "<<match[1]<<" 2: "

    earnings_data._year() = stoi( match[3].str() );
    earnings_data._quarter() = 0;
    // check for share units in title
    nsrUnits = checkForShareUnitsInTitle(titleText);
//LOG_INFO<<"Share units are currently "<<nsrUnits<<" going to parse table...";
  
    _col_num = findColumnToExtract(tree, earnings_data);

    foundRev = extractTotalRevenue(tree, earnings_data, units);
    if (foundRev)
        LOG_INFO << "Succesfully found revenue";

    foundInc = extractNetIncome(tree, earnings_data, units);
    if (foundInc)
        LOG_INFO << "Succesfully found Net Income";

    foundEps = extractEps(tree, earnings_data, units);
    if (foundEps)
        LOG_INFO << "Succesfully found eps";

    foundNsr = extractNumShares(tree, earnings_data, units, nsrUnits);
    if (foundNsr)
        LOG_INFO << "Succesfully shares outstanding";
}

string 
Parser::extractFiscalDateFromReport(string& report)
{
    string tableStr = extractFirstTableStr(report); 
    XmlElement* tree = buildXmlTree(tableStr);
    string date="";

    string trTitle("Fiscal Year End Date");
    string tagName("tr");
    size_t* counter = new size_t;
    *counter=0;
    XmlElement* dataLine = tree->tagWithText(tagName,trTitle,1,counter);
    if (dataLine==NULL)
    {
        LOG_ERROR << "Could not get FYED for stock\n";
        return "";
    }
    string dataText = dataLine->text();
    LOG_INFO << " Got fiscal year end date of " << dataText<< "\n";
    
    boost::regex pattern("\\d\\d-\\d\\d");
    boost::smatch match;
    if (boost::regex_search(dataText, match, pattern) )
    {
        date = match[0];
        LOG_INFO << "Saving end date string of "<< date<<"\n";
    } else
        LOG_ERROR << "Failed to get end date string from cover report";

    return date;
}


vector<float> 
Parser::getAnualEps(XmlElement* tree, bool singleYear){
    string trTitle("(diluted|dilution)");
    string tagName("tr");
    vector<float> retVals;
    LOG_INFO<<"getAnualEps() called";

    size_t* counter = new size_t;
    *counter=0;
    XmlElement* dataLine = tree->tagWithText(tagName,trTitle,1,counter);

    if ( dataLine == NULL )
    {
        LOG_ERROR << "Could not extract " << tagName 
                  << " with text matching pattern " << trTitle;
        return retVals;
    }
    string dataText = dataLine->text();

    LOG_INFO << " \n Data line text is : " << dataText;
    if ( containsNoData( dataText ))
    {
        *counter=0;
        XmlElement* dataLine = tree->tagWithText(tagName,trTitle,2,counter);
        if ( dataLine != NULL)
        {
            dataText = dataLine->text();
            LOG_INFO << " \n Data line text is now: " << dataText;
        }else{
            LOG_ERROR << " \n Could not retrieve eps data";
            return retVals;
        }
    }

    boost::regex pattern("\\(?(\\d+)(.\\d+)?\\)?");
    boost::sregex_iterator mit(dataText.begin(), dataText.end(), pattern);
    boost::sregex_iterator mEnd;

    for(; mit != mEnd; ++mit)
    {
        string val = (*mit)[0].str();
        LOG_INFO << "\n Adding extracted val : " << val;
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
    /*
    string tagName("table");
    XmlElement* tab = tree->firstNodebyName(tagName);
    */
    trIterator iter(tree);
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
    trIterator iterb(tree);

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
    size_t* counter = new size_t;;
    *counter=0;
    XmlElement* dataLine = tree->tagWithText(tagName,trTitlePattern,1,counter);
    vector<string> retVals;

    if ( dataLine == NULL )
    {
        LOG_ERROR << "Could not extract " << tagName 
                  << " with text matching pattern " << trTitlePattern;
        return retVals;
    }
    string dataText = dataLine->text();
    LOG_INFO << " \n Data line text is : " << dataText;

    boost::regex pattern("(\\()?(\\d+)([,.]?\\d+)?(,?)(\\d+)?(\\))?");
    boost::sregex_iterator mit(dataText.begin(), dataText.end(), pattern);
    boost::sregex_iterator mEnd;


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
    if (startPos == string::npos)
    {   
        LOG_INFO << "edgarResultsTableToTree() could not find start postition "
                 << startSearchS << "in content page "<<page;
        return NULL;
    }
    startPos = page.find(openTag, startPos);
    if (startPos == string::npos)
    {   
        LOG_INFO << "edgarResultsTableToTree() could not find table start";
        return NULL;
    }
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
    boost::regex amend_pattern("Amend");
    boost::smatch match0;
    if ( boost::regex_search(text, match0, amend_pattern) )
    {
        LOG_INFO << "\n This is an AMEND to a previous statement: "<< text;
        // This is a BAD sign for the company...
        return NULL;
    }

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
    if (table==NULL)
    {
        LOG_ERROR << "Failed to build table from search results page";
        return acns;
    }
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

void 
Parser::set_stock(DMMM::O_Stock& stock)
{
    _stock = stock;
}

