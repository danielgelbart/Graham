#include <fstream>
#include <streambuf>
#include <iostream>
#include <map>
#include <math.h>
//#include <limits>
#include "Utils.hpp"
#include "Logger.h"
#include "Tokenizer.h"
#include "T_Stock.hpp"
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
countDecimals(const string& num)
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
    if (_node == NULL)
        LOG_ERROR << "node is NULL when calling at()";
    size_t num_children = _node->_children.size();
    if ( i >= num_children )
        return NULL;
    // iterate over children
    // advance counter only if text!=""
    regex blank_pattern("\\s:\\s\\[\\d\\]|\\s*");
    size_t counter = 0;
    for( auto it = _node->_children.begin(); it != _node->_children.end(); ++it )
    {
        string text = (*it)->text();
        //LOG_INFO << "TEXT is "<< (*it)->text() << " COUNTER is" << to_string(counter);
        if ( (counter == i) &&
             ( ! regex_match(text,blank_pattern) ) )
            return *it;

        if (! regex_match(text,blank_pattern) )
            counter++;
     }
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
//    LOG_INFO << "comparing to tag with text: "<< text()<<"\n";
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
XmlElement::attrText()
{
    string rText = "";

    for( auto it = _attributes.begin(); it != _attributes.end(); ++it )
        rText += it->first +" = " +it->second + " ";

    for( auto it = _children.begin(); it != _children.end(); ++it )
        rText += (*it)->attrText();

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

    if (filingSummary == "")
    {
        LOG_ERROR << "Cannot procede to handle without filing summary";
        return;
    }

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
        if (docFileString == ""){
            LOG_ERROR << "Failed to retrive document named "<< reportKey
                      << " from filing";
        }else
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

string
adjustForDecimals(string& val, const string& units)
{
    size_t decimals = countDecimals(val);
    string adjusted_units = units;
    val = removeNonDigit(val);
    if( (decimals > 0) &&
        (units.length() > decimals) )
    {   
        adjusted_units = units.substr(0,(units.length() - decimals));
        LOG_INFO <<"units are "<<units<<" and I counted "<<to_string(decimals)
                 <<" for value "<<val<<" so shortend units to "<<adjusted_units;
    }
    return val + adjusted_units;
}

void
writeRevenueToEarnings(O_Ep& ep, string& val, string& units)
{
    // use adjust for decimals
    ep._revenue() = adjustForDecimals(val,units);
}
void
writeIncomeToEarnings(O_Ep& ep, string& val, string& units)
{
    // Handle negative values
    string sign("");
    if (val[0] == '(')
    {
        val = removeParenthasis(val);
        sign = "-";
    }
    ep._net_income() = sign + adjustForDecimals(val,units);
}
void
writeEpsToEarnings(O_Ep& ep, string& val, string& units)
{
    LOG_INFO << "Writing eps of "<<val<<"\n";

    // For BRK with eps in the thuosands
    if ( countDecimals(val) == 0 )
        val = removeNonDigit(val);

    // Handle negative values
    double sign = 1;
    if (val[0] == '(')
    {
        val = removeParenthasis(val);
        sign = -1;
    }
    ep._eps() = stod( val ) * sign;
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
    if ( (regex_search(text, block_pattern)) &&
         (containsNoData( text )) )
    {
        LOG_INFO<<"Found block pattern: "<<block_pattern.str()
                <<" | In line: "<<text;
        return true;
    }
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
        XmlElement* tdnode = tdIt.at(_col_num);
        if (tdnode == NULL){
            LOG_ERROR << "td child at "<< _col_num << " is NULL";
            return false;
        }
        string tdtext = tdnode->text();

        LOG_INFO << "Matching val from text: "<<tdtext;
        if (boost::regex_search(tdtext, match, extract_pattern) )
        {       
            string val = match[0];
            LOG_INFO << "\n extracted val is"<< val;
            func(earnings,val,units);
            return true;
        } else {
            regex tiny_digit = extract_pattern;
            tdtext = removeleadingComma(tdtext);
            if (func == writeIncomeToEarnings)
                tiny_digit.assign("\\(?\\d\\.?\\d?\\)?");
            if (func == writeNsToEarnings )
                tiny_digit.assign("\\d+(\\.\\d+)?");
            if (regex_match(tdtext,tiny_digit)){
                regex_search(tdtext, match, tiny_digit);
                string val = match[0];
                LOG_INFO << "\n extracted val is"<< val << " using 'tiny' pattern";
                func(earnings,val,units);
                return true;
            }
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

void
writeCurrentAssetsToBalance(O_BalanceSheet& bs, string& val, string& units)
{
    // use adjust for decimals
    bs._current_assets() = adjustForDecimals(val,units);
}

void
writeTotalAssetsToBalance(O_BalanceSheet& bs, string& val, string& units)
{
    // use adjust for decimals
    bs._total_assets() = adjustForDecimals(val,units);
}

void
writeCurrentLiabilitiesToBalance(O_BalanceSheet& bs, string& val, string& units)
{
    // use adjust for decimals
    bs._current_liabilities() = adjustForDecimals(val,units);
}

void
writeTotalLiabilitiesToBalance(O_BalanceSheet& bs, string& val, string& units)
{
    // use adjust for decimals
    bs._total_liabilities() = adjustForDecimals(val,units);
}

void
writeLongTermDebtToBalance(O_BalanceSheet& bs, string& val, string& units)
{
    // use adjust for decimals
    bs._long_term_debt() = adjustForDecimals(val,units);
}
void
writeBookValueBalance(O_BalanceSheet& bs, string& val, string& units)
{
    // use adjust for decimals
    bs._book_value() = adjustForDecimals(val,units);
}

size_t 
Parser::findColumnToExtract(XmlElement* tree, size_t year, size_t quarter)
{
    string titleText = get_title_text(tree);
    regex date_pattern("(\\w\\w\\w).? (\\d+)?, (\\d+)?");

// calculate relavent end date from - stock.fye, ep.year, ep.quarter
    date end_date = calculateEndDate(_stock._fiscal_year_end(), year, quarter);

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
Parser::findDefref(trIterator& trIt, regex& defref, regex& num_pattern, string& units,
           DMMM::O_Ep& earnings_data, void(*func)(O_Ep&,string&,string&))
{
    trIt.resetToStart();
    XmlElement* trp;
    bool founddefref = false;
    while( (trp = trIt.nextTr()) != NULL )
    {
        string attr_text = trp->attrText();
        LOG_INFO<<"attr_text is: "<<attr_text;
        if(( founddefref = checkTrPattern(attr_text, defref, units, trp,
                          num_pattern, earnings_data, func)))
        {
            return founddefref;
        }
    }
    return founddefref;
}

bool
Parser::extractTotalRevenue(XmlElement* tree, DMMM::O_Ep& earnings_data,
                    string& units)
{
    trIterator trIt(tree);
    XmlElement* trp = tree;

    bool foundRev(false);
    bool foundRevBlock(false);
    regex num_pattern("\\d+[,\\d]+(.\\d+)?");

    // **** Search for REVENUE using 'defref' html attribute

    regex defref("defref_us-gaap_Revenues");
    if (( foundRev = findDefref(trIt, defref, num_pattern, units,
                                earnings_data, writeRevenueToEarnings )))
    {
        LOG_INFO<<" Successfully found REVENUE using defref_us-gaap_Revenues (1st)";
        return foundRev;
    }

    // This is a less inclusive item, includes "normal course of buisness" revenues,
    // So may not include interest, premiums, etc
    // BDX, ACO use this only, (without total revenue line).
    defref.assign("'defref_us-gaap_SalesRevenueNet'");
    if (( foundRev = findDefref(trIt, defref, num_pattern, units,
                                earnings_data, writeRevenueToEarnings )))
    {
        LOG_INFO<<" Successfully found REVENUE using defref_us-gaap_SalesRevenueNet (2nd)";
        return foundRev;
    }

    // For AA: us-gaap_SalesRevenueGoodsNet
    // They list revenue as "Salses (Q)"
    defref.assign("us-gaap_SalesRevenueGoodsNet");
    if (( foundRev = findDefref(trIt, defref, num_pattern, units,
                                earnings_data, writeRevenueToEarnings )))
    {
        LOG_INFO<<" Successfully found REVENUE using us-gaap_SalesRevenueGoodsNet (3rd)";
        return foundRev;
    }

    //**** Special handling for non-company type stocks

    LOG_INFO << "Stock "<<_stock._ticker()<< " is a : "<< _stock._company_type();
    // If compnay is a REIT - they report interest income
    // e.g. NLY, ANH
    if ( (_stock._company_type() == EnumStockCOMPANY_TYPE::REIT) ||
         (_stock._company_type() == EnumStockCOMPANY_TYPE::FINANCE) )
    {
        LOG_INFO << "Handling "<<_stock._ticker()<< " as a REIT";
        defref.assign("us-gaap_Interest\\w*IncomeOperating|us-gaap_InterestIncomeExpenseNet");
        if (( foundRev = findDefref(trIt, defref, num_pattern, units,
                                    earnings_data, writeRevenueToEarnings )))
        {
            LOG_INFO<<" Successfully found REVENUE using us-gaap_Interest(\\w*)IncomeOperating (REIT/BANK specific)";
            return foundRev;
        }
        // CPT
        defref.assign("gaap_RealEstateRevenueNet");
        if (( foundRev = findDefref(trIt, defref, num_pattern, units,
                                    earnings_data, writeRevenueToEarnings )))
        {
            LOG_INFO<<" Successfully found REVENUE using gaap_RealEstateRevenueNet (REIT specific)";
            return foundRev;
        }
    } // REIT

    LOG_INFO << "Could not find REVENUE using defref. Going to use heuristics" ;

    trIt.resetToStart();
    // first, try to get single line in one shot
    while( (trp = trIt.nextTr()) != NULL )
    {
        string trtext = trp->text();
        LOG_INFO << "\n Handling line - \n" << trtext;

        boost::regex rev_pattern("Total revenue", boost::regex::icase );
        if ( regex_search( trtext, rev_pattern)) {
            foundRev = checkTrPattern( trtext, rev_pattern, units, trp,
                         num_pattern, earnings_data, writeRevenueToEarnings);
            break;
        }
        // INTC, CAR
        rev_pattern.assign("Net revenue",boost::regex::icase);
        if (regex_search( trtext, rev_pattern)){
            foundRev = checkTrPattern( trtext, rev_pattern, units, trp,
                         num_pattern, earnings_data, writeRevenueToEarnings);
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
                // AHC block
                block_pattern.assign("operating Revenue",regex::icase);
                if((foundRevBlock = testBlock(trtext,block_pattern)))
                    continue;
                // ALG, A, ACO block
                block_pattern.assign("net (sales|revenue):?",regex::icase);
                if((foundRevBlock = testBlock(trtext,block_pattern)))
                    continue;

            } else {
                // We are searching WITHIN block, so test if entered a new block
                if( (!regex_search(trtext,num_pattern)) &&
                    containsNoData(trtext) )
                    break;

                // GOOG, IBM, ACN
                regex rev_pattern("^((\\s|:)*Revenues?)", regex::icase );
                // looking for EXACT match here
                tdIterator tdIt(trp);
                string row_title = tdIt.at(0)->text();
                   LOG_INFO<<"row title is |"<<row_title<<"|";
                if (regex_match(row_title,rev_pattern))
                    if ((foundRev = checkTrPattern( trtext, rev_pattern, units, trp,
                                num_pattern, earnings_data, writeRevenueToEarnings)))
                        break;

                // DE Pattern
                rev_pattern.assign("Total", boost::regex::icase );
                if ((foundRev = checkTrPattern( trtext, rev_pattern, units, trp,
                        num_pattern, earnings_data, writeRevenueToEarnings)))
                    break;

                // ATO Pattern
                rev_pattern.assign("operating revenues", boost::regex::icase );
                if ((foundRev = checkTrPattern( trtext, rev_pattern, units, trp,
                        num_pattern, earnings_data, writeRevenueToEarnings)))
                    break;

                // ALG Pattern - Avoid taking "net sales" line, if it is followd
                // By a "total" line - e.g. DE
                // Also, demand EXACT match
                rev_pattern.assign(" : net sales:?", boost::regex::icase );
                if (regex_match(row_title,rev_pattern))
                {
                    foundRev = checkTrPattern( trtext, rev_pattern, units, trp,
                        num_pattern, earnings_data, writeRevenueToEarnings);
                }
            }
        } // while for block find
    } // if !foundRev

    // 3rd Iteration - single line

    if(!foundRev){
        trIt.resetToStart();
        LOG_INFO << "3rd iteration, Looking for revenue in line form";
        while( (trp = trIt.nextTr()) != NULL )
        {
            string trtext = trp->text();

            // APD
            regex rev_pattern(" : sales",boost::regex::icase);
            tdIterator tdIt(trp);
            string row_title = tdIt.at(0)->text();
            LOG_INFO<<"row title is |"<<row_title<<"|";
            if(regex_match(row_title,rev_pattern))
                foundRev = checkTrPattern( trtext, rev_pattern, units, trp,
                    num_pattern, earnings_data, writeRevenueToEarnings);

            if (foundRev)
                break;

            rev_pattern.assign("Operating (revenues?|income)",boost::regex::icase);
            if (regex_search( trtext, rev_pattern)){
                foundRev = checkTrPattern( trtext, rev_pattern, units, trp,
                                           num_pattern, earnings_data, writeRevenueToEarnings);
                break;
            }
            // Added for BDX/MSFT
            rev_pattern.assign("^((\\s|:)*Revenues?)",boost::regex::icase);
            if (regex_search( trtext, rev_pattern)){
                foundRev = checkTrPattern( trtext, rev_pattern, units, trp,
                                           num_pattern, earnings_data, writeRevenueToEarnings);
                break;
            }
            // Added for MMM
            rev_pattern.assign("net sales",boost::regex::icase);
            if (regex_search( trtext, rev_pattern)){
                foundRev = checkTrPattern( trtext, rev_pattern, units, trp,
                                           num_pattern, earnings_data, writeRevenueToEarnings);
                break;
            }
        }// while
     }//if
    return foundRev;
}

bool
Parser::extractNetIncome(XmlElement* tree, DMMM::O_Ep& earnings_data,
                            string& units)
{
    LOG_INFO << "extractNetIncome() called";
    trIterator trIt(tree);
    XmlElement* trp = tree;
    bool foundInc(false);
    regex num_pattern("\\(?\\d+[,\\d]+(.\\d)?\\)?");

    // this defref include cases of 'attributable to'
    // Note - that there is a ' terminating the string.
    // However, there may be multiple such lines in the document
    regex defref("defref_us-gaap_NetIncomeLoss'");
    if (( foundInc = findDefref(trIt, defref, num_pattern, units,
                                earnings_data, writeIncomeToEarnings )))
    {
        LOG_INFO<<" Successfully found NET INCOME using defref defref_us-gaap_NetIncomeLoss' (1st)";
        return foundInc;
    }

    // CAT
    defref.assign("us-gaap_NetIncomeLossAvailableToCommonStockholders");
    if (( foundInc = findDefref(trIt, defref, num_pattern, units,
                                earnings_data, writeIncomeToEarnings )))
    {
        LOG_INFO<<" Successfully found NET INCOME using defref defref_NetIncomeLossAvailableToCommonStockholders (2nd)";
        return foundInc;
    }

    // BSX
    defref.assign("us-gaap_ProfitLoss");
    if (( foundInc = findDefref(trIt, defref, num_pattern, units,
                                earnings_data, writeIncomeToEarnings )))
    {
        LOG_INFO<<" Successfully found NET INCOME using defref defref us-gaap_ProfitLoss (3rd)";
        return foundInc;
    }

    trIt.resetToStart();

    // defref failed, iterate in line mode
    while( (trp = trIt.nextTr()) != NULL )
    {
        string trtext = trp->text();

        // Should exlode term "per share' to avoid matching eps data
        regex exclusion_pattern("per (common )?share", regex::icase);
        if (regex_search(trtext,exclusion_pattern))
            continue;

        // Normal companies use this title:
        // But there is NO 'break' to allow for "attributable to" to overid it
        regex inc_pattern("^((\\s|:)*Net (income|earnings))",boost::regex::icase);
        foundInc = checkTrPattern( trtext, inc_pattern, units, trp,
                       num_pattern, earnings_data, writeIncomeToEarnings);

        // T, F, BRK.A, CVX report income this way:
        inc_pattern.assign("^((\\s|:)*Net (income|earnings) attributable to)",
                          regex::icase);
        exclusion_pattern.assign("noncontrolling", regex::icase);
        if ( regex_search(trtext,inc_pattern) &&
             !regex_search(trtext,exclusion_pattern) )
            if ((foundInc = checkTrPattern( trtext, inc_pattern, units, trp,
                                            num_pattern, earnings_data, writeIncomeToEarnings)))
                break;
        // CAS 2013
        inc_pattern.assign("^((\\s|:)*Net loss)",boost::regex::icase);
        if ((foundInc = checkTrPattern( trtext, inc_pattern, units, trp,
                       num_pattern, earnings_data, writeIncomeToEarnings)))
                break;
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
    //regex num_pattern("\\d+[,\\d]+\\d+(.\\d+)?");
    regex digit_pattern("\\(?(\\d+)(.\\d+)\\)?");
    regex date_pattern("(\\w\\w\\w). (\\d+)?, (\\d+)?");
//    smatch match;

    //LOG_INFO << "tree is \n"<<tree->printXmlTree(0);
    LOG_INFO << "extractEps() called";

    regex defref("EarningsPerShareDiluted");
    if (( foundEps = findDefref(trIt, defref, digit_pattern, units,
                                earnings_data, writeEpsToEarnings )))
    {
        LOG_INFO<<" Successfully found EPS using defref_us_EarningsPerShareDiluted (1st)";
        return foundEps;
    }

    // ALR
    defref.assign("us-gaap_EarningsPerShareBasicAndDiluted");
    if (( foundEps = findDefref(trIt, defref, digit_pattern, units,
                                earnings_data, writeEpsToEarnings )))
    {
        LOG_INFO<<" Successfully found EPS using us-gaap_EarningsPerShareBasicAndDiluted (2nd)";
        return foundEps;
    }

    // ***** handle asset managment that call shares "units"

    LOG_INFO << "Stock "<<_stock._ticker()<< " is a : "<< _stock._company_type();
    // If compnay is a REIT - they report interest income
    // e.g. NLY, ANH
    if (_stock._company_type() == EnumStockCOMPANY_TYPE::ASSET_MNGMT)
    {
        LOG_INFO << "Handling "<<_stock._ticker()<< " as an asset managment partnership/firm";
        defref.assign("NetIncomeLoss\\w*PerOutstandingLimitedPartnershipUnitDiluted");
        if (( foundEps = findDefref(trIt, defref, digit_pattern, units,
                                    earnings_data, writeEpsToEarnings )))
        {
            LOG_INFO<<" Successfully found EPS using " <<
                      "NetIncomeLossPerOutstandingLimitedPartnershipUnitDiluted (ASSET_MNGMT specific)";
            return foundEps;
        }
    } // ASSET_MNGMT

    LOG_INFO << "defref for EPS failed, using heuristics";
    trIt.resetToStart();

    // First, search for block structure
    while( (trp = trIt.nextTr()) != NULL )
    {
        string trtext = trp->text();

        // find Eps block
        if(!foundEps && !foundEpsBlock)
        {
            LOG_INFO<<"Looking for EPS block in text "<<trtext;
            regex eblock_pattern("(Earnings|income) per [\\w\\s]*share",regex::icase);
            regex exclusion_pattern("Basic (Earnings|income) per [\\w\\s]*share",regex::icase);
            if((foundEpsBlock = testBlock(trtext,eblock_pattern)))
            {
                foundEpsBlock = ! regex_search(trtext,exclusion_pattern);
                if(! foundEpsBlock)
                    LOG_INFO<<"Excluding 'Basic' block, continue search";
                continue;
            }
            // Do we need this? - YES! DE, CVX
            eblock_pattern.assign("per share",regex::icase);
            if((foundEpsBlock = testBlock(trtext,eblock_pattern)))
            {
                regex exclusion_pattern("used in per share calculation",regex::icase);
                if(regex_search(trtext,exclusion_pattern))
                    foundEpsBlock = false;

                continue;
            }
            // ABT
            eblock_pattern.assign("diluted earnings per (common )?share",regex::icase);
            if((foundEpsBlock = testBlock(trtext,eblock_pattern)))
                continue;
            // ARX
            eblock_pattern.assign("^((\\s|:)*Net income (\\(loss\\) )?per (common )?share)",regex::icase);
            if((foundEpsBlock = testBlock(trtext,eblock_pattern)))
                continue;
        }
        // find eps IN block
        if(foundEpsBlock)
        {
            LOG_INFO<<"Found eps BLOCK, now locking for line";
            regex eps_pattern("(diluted|dilution)", regex::icase);
            if((foundEps = checkTrPattern(trtext, eps_pattern, units, trp,
                                          digit_pattern, earnings_data, writeEpsToEarnings)))
                break;
            // AOS, ABT
            // NO Break, in case multiple lines (e.g. "discontinues" then total)
            eps_pattern.assign("^((\\s|:)*Net (earnings|income))", regex::icase);
            if(checkTrPattern(trtext, eps_pattern, units, trp,
                              digit_pattern, earnings_data, writeEpsToEarnings))
                foundEps = true;

            eps_pattern.assign("^((\\s|:)*Net (earnings|income) attributable to)", regex::icase);
            regex exclusion_pattern("basic", regex::icase);
            if(( foundEps = checkTrPattern(trtext, eps_pattern, units, trp,
                                           digit_pattern, earnings_data, writeEpsToEarnings)))
            {
                if (regex_search(trtext,exclusion_pattern))
                    continue;
                break;
            }
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
            // For GOOG, AAN, ABT
            eps_pattern.assign("per (common )?share([\\w\\s-]+)(diluted|dilution)",
                                  regex::icase);
            if((foundEps = checkTrPattern(trtext, eps_pattern, units, trp,
                          digit_pattern, earnings_data, writeEpsToEarnings)))
                break;
            // For BRK.A
            eps_pattern.assign("net earnings per share attributable to([\\w\\s]+) shareholders", regex::icase);
            if((foundEps = checkTrPattern(trtext, eps_pattern, units, trp,
                         digit_pattern, earnings_data, writeEpsToEarnings)))
                break;
            // For AMD
            eps_pattern.assign("diluted net income (loss) per share", regex::icase);
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
    regex num_pattern("\\d+[,\\d]+\\d+(.\\d+)?"); // at least 3 non decimal digits
    regex nsunits_pattern("(millions|thousands|billions)",regex::icase);
    regex date_pattern("(\\w\\w\\w). (\\d+)?, (\\d+)?");

    LOG_INFO << "extractNumShares() called";
    regex defref("WeightedAverageNumberOfDilutedSharesOutstanding");
    while( (trp = trIt.nextTr()) != NULL )
    {
        string attr_text = trp->attrText();
        LOG_INFO << "Attr text is: "<<attr_text;
        if ( regex_search( attr_text, defref ) )
        {
            // check for units in trtext
            if(nsrUnits != "")
            {
                string trtext = trp->text();
                nsrUnits = checkForShareUnits(trtext);
            }
            string bothUnits = nsrUnits+"|"+units;
            if ((foundNsr = checkTrPattern(attr_text, defref, bothUnits, trp,
                                           num_pattern, earnings_data, writeNsToEarnings)))
            {
                LOG_INFO<<" Successfully found DILUTED SHARES using defref";
                return true;
            }
        } // if defref regex match
    } // while defref

    trIt.resetToStart();

    // Block form (only)
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
            sblock_pattern.assign("(weighted )?average [\\w\\s]*shares",regex::icase);
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
 
                string bothUnits = nsrUnits+"|"+units;
                foundNsr = checkTrPattern(trtext, ds_pattern, bothUnits, trp,
                                          num_pattern, earnings_data, writeNsToEarnings);
                continue;
            }
        } // find line
    } // while


    //Line form
    if(!foundNsr)
    {
        LOG_INFO<<"Looking for num shares in line form";
        trIt.resetToStart();

        while( (trp = trIt.nextTr()) != NULL )
        {
            string trtext = trp->text();
            // MMM, APD
            regex nsr_pattern("weighted average[3\\w\\s]*common shares outstanding[\\s-]*(diluted|assuming dilution)",
                              regex::icase);
            if (regex_search(trtext,nsr_pattern))
            {
                nsrUnits = checkForShareUnits(trtext);
                string bothUnits = nsrUnits+"|"+units;
                if((foundNsr = checkTrPattern(trtext, nsr_pattern, bothUnits,
                          trp, num_pattern, earnings_data, writeNsToEarnings)))
                    break;
            }
        }//while
    } // if no nsr
    return foundNsr;
}

void
Parser::getNumSharesFromCoverReport(string& report, O_Ep& ep)
{
    if (report ==""){
        LOG_ERROR << "No cover report available. So exusted attempt to "
                  << "read num shares";
        return;
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

    // get GENERAL units from title block
    if (units == "")
    {
        regex u_pattern("(millions|thousands|billions)", regex::icase);
        boost::smatch match1;
        if ( boost::regex_search(titleText, match1, u_pattern) )
        {
            units =  match1[0].str();
            LOG_INFO << "Found units in title, they are "<< units;
        }
    }

    trIterator trIt(tree);
    XmlElement* trp = tree;
    string numshares("");
    regex shares_pattern("Entity Common Stock,? Shares Outstanding", regex::icase);

    while( (trp = trIt.nextTr()) != NULL )
    {
        string trtext = trp->text();
        if (regex_search(trtext,shares_pattern))
        {
            boost::regex pattern("\\d+[,\\d]+(.\\d)?");
            boost::smatch match;
            if (boost::regex_search(trtext, match, pattern) )
            {
                LOG_INFO << "got numshares";
                numshares = match[0];
                LOG_INFO << "Got num shares from cover report. They are: "<< numshares;

            } else{
                LOG_ERROR << "Could not get numshares from cover report";
                return;
            }
            break;
        }
    }
    if (numshares == "")
    {
        LOG_ERROR << "Failed to retrieve num shares from cover report";
        return;
    }
    // Need to add the "|" for units|bunits structure
    units = "|"+units;
    ep._shares_diluted() = false;
    writeNsToEarnings( ep, numshares, units);

}

void 
Parser::parseIncomeTree(XmlElement* tree, DMMM::O_Ep& earnings_data)
{
    bool foundRev(false);
    bool foundInc(false);
    bool foundEps(false);
    bool foundNsr(false);

    string nsrUnits = "";
    string units = "";
    string currency = "";

    // get units from title
    string titleText = getUnitsAndCurrency( tree, units, currency);
    if(titleText == "")
    {
        LOG_ERROR<<"Malformed income table, or at least title is malformed";
        return;
    }
    // check for share units in title
    nsrUnits = checkForShareUnitsInTitle(titleText);
//LOG_INFO<<"Share units are currently "<<nsrUnits<<" going to parse table...";
  
    _col_num = findColumnToExtract(tree, earnings_data._year(), earnings_data._quarter());
    LOG_INFO << "Extractino col num is " << _col_num;

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

bool
Parser::checkTrPattern( string& text, boost::regex& title_pattern,
                        string& units, XmlElement* node,
                        boost::regex& extract_pattern, O_BalanceSheet& balance,
                        void(*func)(O_BalanceSheet&,string&,string&))
{
    boost::smatch match;
    if ( boost::regex_search( text, title_pattern ) )
    {
        LOG_INFO << "\n regex match in text: "<<text<<"\n col to get is: "
                 <<to_string(_col_num);

        // get i'th td from tr
        tdIterator tdIt(node);
        XmlElement* tdnode = tdIt.at(_col_num);
        if (tdnode == NULL){
            LOG_ERROR << "td child at "<< _col_num << " is NULL";
            return false;
        }
        string tdtext = tdnode->text();

        LOG_INFO << "Matching val from text: "<<tdtext;
        if (boost::regex_search(tdtext, match, extract_pattern) )
        {
            string val = match[0];
            LOG_INFO << "\n extracted val is"<< val;
            func(balance,val,units);
            return true;
        } else {
            regex tiny_digit = extract_pattern;
            tdtext = removeleadingComma(tdtext);
            if (func == writeCurrentAssetsToBalance )
                tiny_digit.assign("\\(?\\d\\.?\\d?\\)?");
        }
    }
    return false;
}

bool
Parser::findDefref(trIterator& trIt, regex& defref, regex& num_pattern, string& units,
           DMMM::O_BalanceSheet& balance_data, void(*func)(O_BalanceSheet&,string&,string&))
{
    trIt.resetToStart();
    XmlElement* trp;
    bool founddefref = false;
    while( (trp = trIt.nextTr()) != NULL )
    {
        string attr_text = trp->attrText();
        LOG_INFO<<"attr_text is: "<<attr_text;
        if(( founddefref = checkTrPattern(attr_text, defref, units, trp,
                          num_pattern, balance_data, func)))
        {
            return founddefref;
        }
    }
    return founddefref;
}


bool
Parser::extractCurrentAssets(XmlElement* tree, DMMM::O_BalanceSheet& balance_data,
                    string& units)
{
    trIterator trIt(tree);
    XmlElement* trp = tree;

    bool foundCA(false);
    bool foundCABlock(false);
    regex num_pattern("\\d+[,\\d]+(.\\d+)?");

    // **** Search for CA using 'defref' html attribute
    regex defref("us-gaap_AssetsCurrent");
    if (( foundCA = findDefref(trIt, defref, num_pattern, units,
                                balance_data, writeCurrentAssetsToBalance )))
    {
        LOG_INFO<<" Successfully found Current Assest using us-gaap_AssetsCurrent (1st)";
        return foundCA;
    }

    //**** Special handling for non-company type stocks

    LOG_INFO << "Stock "<<_stock._ticker()<< " is a : "<< _stock._company_type();
    // If compnay is a REIT - they report interest income
    // e.g. NLY, ANH
    if ( (_stock._company_type() == EnumStockCOMPANY_TYPE::REIT) ||
         (_stock._company_type() == EnumStockCOMPANY_TYPE::FINANCE) )
    {
        LOG_INFO << "Handling "<<_stock._ticker()<< " as a REIT - NOT yet implemented";
        defref.assign("");

    } // REIT

    LOG_INFO << "Could not find CURRENT ASSETS using defref. Going to use heuristics" ;

    trIt.resetToStart();
    // first, try to get single line in one shot
    while( (trp = trIt.nextTr()) != NULL )
    {
        string trtext = trp->text();
        LOG_INFO << "\n Handling line - \n" << trtext;

        boost::regex rev_pattern("Total current assets", boost::regex::icase );
        if ( regex_search( trtext, rev_pattern)) {
            foundCA = checkTrPattern( trtext, rev_pattern, units, trp,
                         num_pattern, balance_data, writeCurrentAssetsToBalance);
            break;
        }
    } // while loop over table

    // if not found - iterate in block search mode
    trIt.resetToStart();
    if(!foundCA){
        LOG_INFO << "Looking for current assets by block strocture";
        while( (trp = trIt.nextTr()) != NULL )
        {
            string trtext = trp->text();
            if(!foundCABlock)
            {
                //
                regex block_pattern("current assets",regex::icase);

            } else {
                // We are searching WITHIN block, so test if entered a new block
                if( (!regex_search(trtext,num_pattern)) &&
                    containsNoData(trtext) )
                    break;

                //test line inside block
                regex ca_pattern("^((\\s|:)*total)", regex::icase );

                // looking for EXACT match here
                tdIterator tdIt(trp);
                string row_title = tdIt.at(0)->text();
                   LOG_INFO<<"row title is |"<<row_title<<"|";
                if (regex_match(row_title,ca_pattern))
                    if ((foundCA = checkTrPattern( trtext, ca_pattern, units, trp,
                                num_pattern, balance_data, writeCurrentAssetsToBalance)))
                        break;


            }
        } // while for block find
    } // if !foundCA

    return foundCA;
}

bool
Parser::extractTotalAssets(XmlElement* tree, DMMM::O_BalanceSheet& balance_data,
                    string& units)
{
    trIterator trIt(tree);
    XmlElement* trp = tree;

    bool foundTA(false);
    bool foundTABlock(false);
    regex num_pattern("\\d+[,\\d]+(.\\d+)?");

    // **** Search for TA using 'defref' html attribute
    regex defref("us-gaap_Assets'");
    if (( foundTA = findDefref(trIt, defref, num_pattern, units,
                                balance_data, writeTotalAssetsToBalance )))
    {
        LOG_INFO<<" Successfully found Total Assest using us-gaap_Assets' (1st)";
        return foundTA;
    }

    //**** Special handling for non-company type stocks

    LOG_INFO << "Stock "<<_stock._ticker()<< " is a : "<< _stock._company_type();
    // If compnay is a REIT - they report interest income
    // e.g. NLY, ANH
    if ( (_stock._company_type() == EnumStockCOMPANY_TYPE::REIT) ||
         (_stock._company_type() == EnumStockCOMPANY_TYPE::FINANCE) )
    {
        LOG_INFO << "Handling "<<_stock._ticker()<< " as a REIT - NOT yet implemented";
        defref.assign("");
    } // REIT

    LOG_INFO << "Could not find TOTAL ASSETS using defref. Going to use heuristics" ;

    trIt.resetToStart();
    // first, try to get single line in one shot
    while( (trp = trIt.nextTr()) != NULL )
    {
        string trtext = trp->text();
        LOG_INFO << "\n Handling line - \n" << trtext;

        boost::regex rev_pattern("Total assets", boost::regex::icase );
        if ( regex_search( trtext, rev_pattern)) {
            foundTA = checkTrPattern( trtext, rev_pattern, units, trp,
                         num_pattern, balance_data, writeTotalAssetsToBalance);
            break;
        }
    } // while loop over table

    // if not found - iterate in block search mode
    trIt.resetToStart();
    if(!foundTA){
        LOG_INFO << "Looking for current assets by block strocture";
        while( (trp = trIt.nextTr()) != NULL )
        {
            string trtext = trp->text();
            if(!foundTABlock)
            {
                //
                regex block_pattern("^((\\s|:)*assets)",regex::icase);

            } else {
                // We are searching WITHIN block, so test if entered a new block
                if( (!regex_search(trtext,num_pattern)) &&
                    containsNoData(trtext) )
                    break;

                //test line inside block
                regex ca_pattern("^((\\s|:)*total assets)", regex::icase );

                // looking for EXACT match here
                tdIterator tdIt(trp);
                string row_title = tdIt.at(0)->text();
                   LOG_INFO<<"row title is |"<<row_title<<"|";
                if (regex_match(row_title,ca_pattern))
                    if ((foundTA = checkTrPattern( trtext, ca_pattern, units, trp,
                                num_pattern, balance_data, writeTotalAssetsToBalance)))
                        break;
            }
        } // while for block find
    } // if !foundTA
    return foundTA;
}

bool
Parser::extractCurrentLiabilities(XmlElement* tree, DMMM::O_BalanceSheet& balance_data,
                    string& units)
{
    trIterator trIt(tree);
    XmlElement* trp = tree;

    bool foundCL(false);
    bool foundCLBlock(false);
    regex num_pattern("\\d+[,\\d]+(.\\d+)?");

    // **** Search for CL using 'defref' html attribute
    regex defref("us-gaap_LiabilitiesCurrent");
    if (( foundCL = findDefref(trIt, defref, num_pattern, units,
                                balance_data, writeCurrentLiabilitiesToBalance )))
    {
        LOG_INFO<<" Successfully found Current Liabilities using us-gaap_LiabilitiesCurrent (1st)";
        return foundCL;
    }

    //**** Special handling for non-company type stocks

    LOG_INFO << "Stock "<<_stock._ticker()<< " is a : "<< _stock._company_type();
    // If compnay is a REIT - they report interest income
    // e.g. NLY, ANH
    if ( (_stock._company_type() == EnumStockCOMPANY_TYPE::REIT) ||
         (_stock._company_type() == EnumStockCOMPANY_TYPE::FINANCE) )
    {
        LOG_INFO << "Handling "<<_stock._ticker()<< " as a REIT - NOT yet implemented";
        defref.assign("");
    } // REIT

    LOG_INFO << "Could not find CURRENT Liabilities using defref. Going to use heuristics" ;

    trIt.resetToStart();
    // first, try to get single line in one shot
    while( (trp = trIt.nextTr()) != NULL )
    {
        string trtext = trp->text();
        LOG_INFO << "\n Handling line - \n" << trtext;

        boost::regex tl_pattern("Total liabilities", boost::regex::icase );
        if ( regex_search( trtext, tl_pattern)) {
            foundCL = checkTrPattern( trtext, tl_pattern, units, trp,
                         num_pattern, balance_data, writeCurrentLiabilitiesToBalance );
            break;
        }
    } // while loop over table

    // if not found - iterate in block search mode
    trIt.resetToStart();
    if(!foundCL){
        LOG_INFO << "Looking for current assets by block strocture";
        while( (trp = trIt.nextTr()) != NULL )
        {
            string trtext = trp->text();
            if(!foundCLBlock)
            {
                //
                regex block_pattern("^((\\s|:)*current liabilities)",regex::icase);

            } else {
                // We are searching WITHIN block, so test if entered a new block
                if( (!regex_search(trtext,num_pattern)) &&
                    containsNoData(trtext) )
                    break;

                //test line inside block
                regex ca_pattern("^((\\s|:)*total current liabilities)", regex::icase );

                // looking for EXACT match here
                tdIterator tdIt(trp);
                string row_title = tdIt.at(0)->text();
                   LOG_INFO<<"row title is |"<<row_title<<"|";
                if (regex_match(row_title,ca_pattern))
                    if ((foundCL = checkTrPattern( trtext, ca_pattern, units, trp,
                                num_pattern, balance_data, writeCurrentLiabilitiesToBalance)))
                        break;
            }
        } // while for block find
    } // if !foundCL
    return foundCL;
}

bool
Parser::extractTotalLiabilities(XmlElement* tree, DMMM::O_BalanceSheet& balance_data,
                    string& units)
{
    trIterator trIt(tree);
    XmlElement* trp = tree;

    bool foundTL(false);
    bool foundTLBlock(false);
    regex num_pattern("\\d+[,\\d]+(.\\d+)?");

    // **** Search for TL using 'defref' html attribute
    regex defref("us-gaap_Liabilities'");
    if (( foundTL = findDefref(trIt, defref, num_pattern, units,
                                balance_data, writeTotalLiabilitiesToBalance )))
    {
        LOG_INFO<<" Successfully found Total Liabilities using us-gaap_Liabilities' (1st)";
        return foundTL;
    }

    //**** Special handling for non-company type stocks

    LOG_INFO << "Stock "<<_stock._ticker()<< " is a : "<< _stock._company_type();
    // If compnay is a REIT - they report interest income
    // e.g. NLY, ANH
    if ( (_stock._company_type() == EnumStockCOMPANY_TYPE::REIT) ||
         (_stock._company_type() == EnumStockCOMPANY_TYPE::FINANCE) )
    {
        LOG_INFO << "Handling "<<_stock._ticker()<< " as a REIT - NOT yet implemented";
        defref.assign("");
    } // REIT

    LOG_INFO << "Could not find TOTAL Liabilities using defref. Going to use heuristics" ;

    trIt.resetToStart();
    // first, try to get single line in one shot
    while( (trp = trIt.nextTr()) != NULL )
    {
        string trtext = trp->text();
        LOG_INFO << "\n Handling line - \n" << trtext;

        regex tl_pattern("Total liabilities", boost::regex::icase );
        regex exclution("equity", regex::icase);
        if (regex_search( trtext, exclution))
            continue;
        if ( regex_search( trtext, tl_pattern)) {
            foundTL = checkTrPattern( trtext, tl_pattern, units, trp,
                         num_pattern, balance_data, writeTotalLiabilitiesToBalance );
            break;
        }
    } // while loop over table

    // if not found - iterate in block search mode
    trIt.resetToStart();
    if(!foundTL){
        LOG_INFO << "Looking for current assets by block strocture";
        while( (trp = trIt.nextTr()) != NULL )
        {
            string trtext = trp->text();
            if(!foundTLBlock)
            {
                //
                regex block_pattern("^((\\s|:)*liabilities)",regex::icase);

            } else {
                // We are searching WITHIN block, so test if entered a new block
                if( (!regex_search(trtext,num_pattern)) &&
                    containsNoData(trtext) )
                    break;

                //test line inside block
                regex ca_pattern("^((\\s|:)*total liabilities)", regex::icase );

                // looking for EXACT match here
                tdIterator tdIt(trp);
                string row_title = tdIt.at(0)->text();
                   LOG_INFO<<"row title is |"<<row_title<<"|";
                if (regex_match(row_title,ca_pattern))
                    if ((foundTL = checkTrPattern( trtext, ca_pattern, units, trp,
                                num_pattern, balance_data, writeTotalLiabilitiesToBalance)))
                        break;
            }
        } // while for block find
    } // if !foundTL
    return foundTL;
}

bool
Parser::extractBookValue(XmlElement* tree, O_BalanceSheet& balance_data, string& units)
{
    trIterator trIt(tree);
    XmlElement* trp = tree;

    bool foundBV(false);
    regex num_pattern("\\d+[,\\d]+(.\\d+)?");

    // **** Search for TL using 'defref' html attribute
    regex defref("us-gaap_StockholdersEquity");
    if (( foundBV = findDefref(trIt, defref, num_pattern, units,
                                balance_data, writeBookValueBalance )))
    {
        LOG_INFO<<" Successfully found Total Liabilities using us-gaap_StockholdersEquity (1st)";
        return foundBV;
    }

    //**** Special handling for non-company type stocks
    LOG_INFO << "Stock "<<_stock._ticker()<< " is a : "<< _stock._company_type();
    // If compnay is a REIT - they report interest income
    // e.g. NLY, ANH
    if ( (_stock._company_type() == EnumStockCOMPANY_TYPE::REIT) ||
         (_stock._company_type() == EnumStockCOMPANY_TYPE::FINANCE) )
    {
        LOG_INFO << "Handling "<<_stock._ticker()<< " as a REIT - NOT yet implemented";
        defref.assign("");
    } // REIT

    LOG_INFO << "Could not find TOTAL Liabilities using defref. Going to use heuristics" ;

    trIt.resetToStart();
    // first, try to get single line in one shot
    while( (trp = trIt.nextTr()) != NULL )
    {
        string trtext = trp->text();
        LOG_INFO << "\n Handling line - \n" << trtext;

        boost::regex bv_pattern("Total (Stock|Share)holder's Equity", boost::regex::icase );
        if ( regex_search( trtext, bv_pattern)) {
            foundBV = checkTrPattern( trtext, bv_pattern, units, trp,
                         num_pattern, balance_data, writeBookValueBalance );
            break;
        }
    } // while loop over table

    return foundBV;
}

bool
Parser::extractLongTermDebt(XmlElement* tree, DMMM::O_BalanceSheet& balance_data,
                    string& units)
{
    trIterator trIt(tree);
    XmlElement* trp = tree;

    bool foundLTD(false);
    bool foundLTDBlock(false);
    regex num_pattern("\\d+[,\\d]+(.\\d+)?");

    // **** Search for TL using 'defref' html attribute
    regex defref("us-gaap_LongTermDebtNoncurrent");
    if (( foundLTD = findDefref(trIt, defref, num_pattern, units,
                                balance_data, writeLongTermDebtToBalance )))
    {
        LOG_INFO<<" Successfully found Long Term Debt using us-gaap_LongTermDebtNoncurrent (1st)";
        return foundLTD;
    }

    //**** Special handling for non-company type stocks

    LOG_INFO << "Stock "<<_stock._ticker()<< " is a : "<< _stock._company_type();
    // If compnay is a REIT - they report interest income
    // e.g. NLY, ANH
    if ( (_stock._company_type() == EnumStockCOMPANY_TYPE::REIT) ||
         (_stock._company_type() == EnumStockCOMPANY_TYPE::FINANCE) )
    {
        LOG_INFO << "Handling "<<_stock._ticker()<< " as a REIT - NOT yet implemented";
        defref.assign("");
    } // REIT

    LOG_INFO << "Could not find TOTAL Liabilities using defref. Going to use heuristics" ;

    trIt.resetToStart();
    // first, try to get single line in one shot
    while( (trp = trIt.nextTr()) != NULL )
    {
        string trtext = trp->text();
        LOG_INFO << "\n Handling line - \n" << trtext;

        boost::regex ltd_pattern("long-term (debt|borrowings)", boost::regex::icase );
        if ( regex_search( trtext, ltd_pattern)) {
            foundLTD = checkTrPattern( trtext, ltd_pattern, units, trp,
                         num_pattern, balance_data, writeLongTermDebtToBalance );
            break;
        }
    } // while loop over table

    // if not found - iterate in block search mode
    trIt.resetToStart();
    if(!foundLTD){
        LOG_INFO << "Looking for current assets by block strocture";
        while( (trp = trIt.nextTr()) != NULL )
        {
            string trtext = trp->text();
            if(!foundLTDBlock)
            {
                //
                regex block_pattern("^((\\s|:)*liabilities)",regex::icase);

            } else {
                // We are searching WITHIN block, so test if entered a new block
                if( (!regex_search(trtext,num_pattern)) &&
                    containsNoData(trtext) )
                    break;

                //test line inside block
                regex ltd_pattern("^((\\s|:)*long-term)", regex::icase );

                // looking for EXACT match here
                tdIterator tdIt(trp);
                string row_title = tdIt.at(0)->text();
                   LOG_INFO<<"row title is |"<<row_title<<"|";
                if (regex_match(row_title,ltd_pattern))
                    if ((foundLTD = checkTrPattern( trtext, ltd_pattern, units, trp,
                                num_pattern, balance_data, writeLongTermDebtToBalance)))
                        break;
            }
        } // while for block find
    } // if !foundLTD
    return foundLTD;
}

void
calculate_book_value(DMMM::O_BalanceSheet& balance_data)
{
    LOG_INFO << "Calculaing book value in method Parser::calculate_book_value()";
    LOG_INFO << "total assets are" << balance_data._total_assets() << " and TL are: " << balance_data._total_liabilities();
    long assets = stol(balance_data._total_assets());
    long liabilities = stol(balance_data._total_liabilities());
    balance_data._book_value() = to_string(assets - liabilities);
    balance_data._calculated_bv() = true;
}

void
calculate_total_liabilities(DMMM::O_BalanceSheet& balance_data)
{
    LOG_INFO << "Calculaing total liabilities in method Parser::calculate_total_liabiities()";
    long assets = stol(balance_data._total_assets());
    long book_value = stol(balance_data._book_value());
    balance_data._total_liabilities() = to_string(assets - book_value);
    balance_data._calculated_tl() = true;
}


void
Parser::parseBalanceTree(XmlElement* tree, DMMM::O_BalanceSheet& balance_data)
{
    // #  current_assets      :string(255)
    //#  total_assets        :string(255)
    //#  current_liabilities :string(255)
    //#  total_liabilities   :string(255)
    //#  long_term_debt      :string(255)
    //#  net_tangible_assets :string(255)
    //-calculate:
    //#  book_value          :string(255)

    string units = "";
    string currency = "";

    // get units from title
    string titleText = getUnitsAndCurrency( tree, units, currency);
    if(titleText == "")
    {
        LOG_ERROR<<"Malformed income table, or at least title is malformed";
        return;
    }

    _col_num = findColumnToExtract(tree, balance_data._year(), balance_data._quarter() );
    LOG_INFO << "Extractino col num for balance sheets is " << _col_num;


    bool has_ca = extractCurrentAssets(tree, balance_data, units);
    bool has_ta = extractTotalAssets(tree, balance_data, units);
    bool has_cl = extractCurrentLiabilities(tree, balance_data, units);
    bool has_tl = extractTotalLiabilities(tree, balance_data, units);
    extractLongTermDebt(tree, balance_data, units);
    if (has_ca && has_cl)
        _stock._has_currant_ratio() = true;

    bool has_bv = extractBookValue(tree, balance_data, units);

    if (!has_tl && has_bv && has_ta)
        calculate_total_liabilities(balance_data);

    if (!has_bv && has_ta && has_tl)
        calculate_book_value(balance_data);
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
    date endate = convertFyedStringToDate((report_date.year() - 1), _stock._fiscal_year_end());
    acn_rec->set_quarter_from_date(endate);

    LOG_INFO << "Extracted ACN: "<< acn << " filed on date" << report_date
                << "for quarter " << acn_rec->_quarter;
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

