#include <fstream>
#include <streambuf>
#include <iostream>
#include <map>
#include <math.h>
//#include <limits>
#include <exception>
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
//    cout << "   Has " << _children.size() << " children";
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

/* only searched for start at self and children */
tagIterator::tagIterator(XmlElement* node, string start, string tag_name):_i(0){
    _start = start;
    _tag_name = tag_name;
    _node = node;
    if(node == NULL){
        _node = node;
    }else{
        if( node->_tagName == start){
            _node = node;
        }else{
            for( auto it = node->_children.begin() ;
                 it!= node->_children.end(); ++it )
                if( (*it)->_tagName == start)
                {
                    _node = *it;
                    break;
                }
            } // else2
        }   // else1
}

XmlElement*
tagIterator::nextTag()
{
    //LOG_INFO << "tagIterator: In tag named"<< _node->_tagName<< "\n";

    size_t num_children = _node->_children.size();
    if ( _i >= num_children )
        return NULL;

    while ( _node->_children[_i]->_tagName != _tag_name)
    {
        //LOG_INFO << "tagIterator: looking at tag named"<< _node->_children[_i]->_tagName << "\n";
        _i = _i + 1;
        if ( _i >= num_children )
            return NULL;
    }
    _i = _i + 1;
    return _node->_children[_i-1];
}

XmlElement*
tagIterator::at(size_t i, bool exact_match)
{
    if (_node == NULL)
        LOG_ERROR << "node is NULL when calling at()";

    size_t num_children = _node->_children.size();
    if ( i >= num_children )
        return NULL;

    if (exact_match)
        return _node->_children[i];

    // iterate over children
    // advance counter only if text!=""
    regex blank_pattern("\\s:\\s(\\[\\d\\]|&#160;)|\\s*");
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
XmlElement::mytext()
{
    return _text;
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

/*
Identifiys from the two first title rows, the specific column with data for relavent date
Decides if this is an exact mathc (i.e counting non data columns)
*/
bool
find_data_column(XmlElement* tree, date end_date, size_t* extraction_col, bool* exact_match, bool quarterly = false)
{
    LOG_INFO << "Going to find column with data for period ending "<< end_date;
    vector<XmlElement*>* elements = new vector<XmlElement*>;
    string tagName("tr");
    tree->getNodes(tagName, 2, elements);

    tagIterator thIt((*elements)[0], string("tr"), string("th"));
    XmlElement* thp;

    size_t number_of_cols = 0, start_of_range = 1, end_of_range = 1;
    size_t ths_examined = 0;

    regex months_pattern("(12|twelve) Months Ended", regex::icase);
    if(quarterly)
        months_pattern.assign("(3|three) Months Ended", regex::icase);

    // iteration over first row of 'th' titles
    while( (thp = thIt.nextTag() ) != NULL )
    {
        LOG_INFO << "Examining 'th' with ATtributes are| "<< thp->attrText() << " Text is| " << thp->text() << "\n";

        // add th to second row, if this th spans it
        if (thp->span_count(string("row")) > 1){
            LOG_INFO << "'th' number "<< ths_examined << " has a rowspan into next row. Adding the in row 2";
            XmlElement* new_th = new XmlElement(thp->_tagName);

            string new_attr_str = string("colspan=\"") + thp->_attributes["colspan"] + "\"";
            new_th->addAttr( new_attr_str );
            auto it = (*elements)[1]->_children.begin();
            (*elements)[1]->_children.insert(it + ths_examined,new_th);
        }

        ths_examined++;

        string th_text = thp->text();
        size_t colspan = 1; // here?
        colspan = thp->span_count(string("col"));

        if(regex_search( th_text, months_pattern)){
            start_of_range = number_of_cols;
            end_of_range = number_of_cols + colspan - 1;
            LOG_INFO << "FOUND RANGE! '"<< months_pattern.str() << "' matheces text: "
                     << th_text <<  " and covers colums starting at col "
                     << start_of_range << " and ending at col" << end_of_range;
            break;
        }
        number_of_cols += colspan;
    }

    tagIterator thIt2((*elements)[1], string("tr"), string("th"));
    regex date_pattern("(\\w\\w\\w).? (\\d+)?, (\\d+)?");

    size_t column_counter = 0;
    *extraction_col = 1;

    // allow to match to a date range 1 day +/- of 'end_date'
    date_period end_date_range(end_date - days(1), end_date + days(1) );

    // For each th encountered in the priouvous loop, remove exxess
    while( (thp = thIt2.nextTag() ) != NULL )
    {
        size_t th_col_span = thp->span_count(string("col"));
        LOG_INFO << "iterating over second row. th counter is: " << column_counter << " th text is: "<< thp->text();

        if (th_col_span > 1)
            *exact_match = true;

        if (column_counter < start_of_range){
            column_counter += th_col_span;
            continue;
        }// not in range yet

        if ((column_counter >= start_of_range) && (column_counter <= end_of_range)){
            LOG_INFO << "Current Examind th is in found ragne for date";
            string th_text = thp->text();
            sregex_iterator mit(th_text.begin(), th_text.end(), date_pattern);
            sregex_iterator mEnd;

            for(size_t i = 0; mit != mEnd; ++mit){

                string dateStr = (*mit)[0].str();
                date rep_date = convertFromDocString(dateStr);

                if (end_date_range.contains(rep_date))
                {
                    LOG_INFO << "Date matches the requested report date, col num found is is "<< (column_counter+i) ;
                    *extraction_col = column_counter + i;
                    return true;
                }
                 ++i; // if there are more dates in the string
            }
        }// search for date withing range

        if (column_counter > end_of_range){
            LOG_ERROR << "Could not find extraction data column number in table within range of columns";
            return false;
        }

    } //while - second row iteration

    LOG_ERROR << "Itereated over all title columns buth could not find extraction column:( ";
    return false;
}

/*
 DEPRECATED in favor of find_data_column()
Retruns the range of columns that span the title "3 months ended"
first column is number 0
Range is inclusive: (1 - 5) includes columns 1 and 5.
*/
bool
find_columns_range(XmlElement* tree, size_t* start_of_range, size_t* end_of_range)
{

    LOG_INFO << "Going to find range of collumns that are marked as '3 months ending' (range)";
    vector<XmlElement*>* elements = new vector<XmlElement*>;
    string tagName("tr");
    tree->getNodes(tagName, 2, elements);

    tagIterator thIt((*elements)[0], string("tr"), string("th"));
    XmlElement* thp;
    //size_t months = 3;
    size_t number_of_cols = 0;
    size_t ths_examined = 0;
    bool in_range(false);
    regex months_pattern("(3|three) Months Ended", regex::icase);
    regex span_pattern("colspan = (\\d)");
    boost::smatch match;

    while( (thp = thIt.nextTag() ) != NULL )
    {
        ths_examined++;

        string th_text = thp->text();
        string attrs = thp->attrText();

        LOG_INFO << " ATtributes are| "<< attrs << " Text is| " << th_text << "\n";
        if(regex_search( th_text, months_pattern))
            in_range = true;           ;// count months

             if (boost::regex_search(attrs, match, span_pattern) )
        {
            LOG_INFO << "Found colspan match mathc[0] is| "<< match[0] << " and match[1] is "<< match[1]<< "\n";
            string val = match[1];
            size_t range_span = stoi(val);
            if (in_range){
                *start_of_range = number_of_cols;
                *end_of_range = number_of_cols + range_span - 1;
                LOG_INFO << "FOUND RANGE! it starts at col "<< *start_of_range << " and ends at " << *end_of_range;
                return true;
            }
            number_of_cols += range_span;
        }
    }

    // Iterate over Look at second tr element
    regex date_pattern("\\w\\w\\w\\s\\d\\d,\\s\\d\\d\\d\\d", regex::icase);
    tagIterator thIt2((*elements)[1], string("tr"), string("th"));
    // For each th encountered in the priouvous loop, remove exxess
    while( (thp = thIt2.nextTag() ) != NULL )
    {
        string th_text = thp->text();
        string attrs = thp->attrText();
        if (regex_search( th_text, date_pattern)){
            LOG_INFO << "Second title row contains a date: "<< th_text;
            if (boost::regex_search(attrs, match, span_pattern) )
            {
                LOG_INFO << "Found colspan match for date: "<< match[0] << " and match[1] is "<< match[1]<< "\n";
                string val = match[1];
                size_t range_span = stoi(val);
                if (range_span > 1)
                     ;  // recomend no skip
            }
        }

    }
    LOG_INFO << "Did not succeed in locating range :( ";
    // Did not succeed in finding the range :(
    return false;
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

string
Parser::readReportHtmlNameFromRepTag(XmlElement* report)
{
    string doc_name = "";
    string doc_tag("HtmlFileName");
    XmlElement* node = report->getFirstChild(doc_tag);
    if(node != NULL)
        doc_name = node->_text;

    if (doc_name == "")
    {
        doc_tag = "XmlFileName";
        XmlElement* node = report->getFirstChild(doc_tag);
        if(node != NULL)
            doc_name = node->_text;
    }
    return doc_name;
}

/* This is identicla logic to tokenizer's parseing of filingsummary BUT:
 * Some compnaies have attrs in their 'Report' tags making strin parsing fail
 * This method parses the report as XmlElement
 */
void
Parser::getReportDocNames(string& filingSummary,map<ReportType,string>* reports_map){

    LOG_INFO << "PARSER::getReportDocNames() called\n";
    //LOG_INFO << "fiingsummary: \n" << filingSummary;
    auto filtree = buildXmlTree(filingSummary);
    string startTagName("MyReports"), tagStrItName("Report");
    XmlElement* filingRepTree = filtree->getFirstChild(startTagName);
    tagIterator filingReportsIt(filingRepTree, startTagName, tagStrItName);
  //  filingRepTree->printXmlTree(0);

    bool foundIncomeRep(false);
    bool foundBalanceRep(false);
    bool foundCoverRep(false);

//    string delimiter("<Report>");
    string tagName("ShortName");
    string tagCatName("MenuCategory");
    XmlElement* report;
    string reportName = "";

    /* TODO SEC do NOT use report names to find them
     * Cover and financial reports are parsed out based on meta information
     * Use this information instead of report name
     */

    //NOTE - ARG call their cover report "DEI Document"
    // Not sure if to adjust regex for this single weirdo

    // BKH - call it "Document Information Document"
    // BUT - Edgar finds it OK. SO - maybe there is some additional markup used to find the cover document!!!
    boost::regex cover_pattern(
        "(Document (and )?Entity (Supplemental )?Information|DEI )(Information )?(Document )?",
        boost::regex_constants::icase);

    // CALX - call their statment: "Consolidated statments of comprehensive Loss"!!!
    // CAT are unique - they use - "Consolidated Results of Operations"
    // CLF - "condensed statement of operations" - NOT going to get that
    // LOW last phrase: 'Consolidated Statements of Current and Retained Earnings'
    boost::regex income_pattern(
        "(consolidated )?(statements? of (consolidated )?(\\(loss\\) )?(earnings|(net )?income|operations|loss)|results of operations|Consolidated Statements of Current and Retained Earnings)",
        boost::regex_constants::icase);

    boost::regex balance_pattern(
        "(consolidated )?(and sector )?((statements? of)? financial (position|condition)|balance sheets?)",
        boost::regex_constants::icase);

    bool hasStatementsCat(false);

//TODO - hits seg fault, needs to iterate correctly, possibly searching for start incorrectly
    while( (report = filingReportsIt.nextTag()) != NULL)
    {
        XmlElement* nameNode = report->getFirstChild(tagName);
        reportName = nameNode->mytext();
        LOG_INFO << "Examing report named: "<< reportName << "\n";
        //The follwoing segment checks if in the filingsummary, reprots are market as 'Statments'
        // If they are, we want to search only within these marked reports
        XmlElement* catNode = report->getFirstChild(tagCatName);
        string category = "";
        if(catNode != NULL){
        category = catNode->mytext();
        string catName = "Statements";

        if ( !hasStatementsCat && (category == catName)){
            LOG_INFO << "Entering report listings for Statments in Filingsummarg\n";
            hasStatementsCat = true;
        }

        if ( hasStatementsCat &&  (category != catName)){
            LOG_INFO << "Previously found reports for Statments, next reports are NOT for stamtnest, so stopping search\n";
            break;
        }
        }
        // TODOcheck that name does NOT inclue (Parenthetical)
        //LOG_INFO << "\n Handling report named: " << reportName << "\n";

        if ((!foundCoverRep && (category == "Cover")) ||
            (!foundCoverRep && boost::regex_search(reportName, cover_pattern)))
        {
            foundCoverRep = true;
            LOG_INFO << "FOUND COVER REPORT\n"<< reportName << "\n";
            reports_map->insert( pair<ReportType,string>(
                                     ReportType::COVER,
                                     readReportHtmlNameFromRepTag(report)) );
        }
        if (!foundIncomeRep && boost::regex_search(reportName, income_pattern))
        {
            // continue of treating "Parenthetical"
            LOG_INFO << "FOUND INCOME REPORT MATCH!\n"<< reportName << "\n";
            boost::regex income_exclution("(Derivative| Impact on)");
            if (regex_search(reportName, income_exclution)){
                LOG_INFO << "Report name matched exclution string for income reports - contineu looking";
            }else{
                foundIncomeRep = true;
                reports_map->insert( pair<ReportType,string>(
                                         ReportType::INCOME,
                                         readReportHtmlNameFromRepTag(report)) );
            }
        }
        if (!foundBalanceRep && boost::regex_search(
                    reportName, balance_pattern))
        {
            foundBalanceRep = true;
            LOG_INFO << "FOUND BALANCE REPORT MATCH!\n"<< reportName << "\n";
            reports_map->insert( pair<ReportType,string>(
                                     ReportType::BALANCE,
                                     readReportHtmlNameFromRepTag(report)) );
        }

        if (foundBalanceRep && foundIncomeRep)
            break;
    }
    if(!foundIncomeRep){
        LOG_INFO<<"------------------------Could NOT find Income statement, searching AGAIN ------------------\n";

        income_pattern.assign("(consolidated (condensed )?(statements? of )?comprehensive (income|operations|loss)|(and sector )?income statements?)",
            boost::regex_constants::icase);

        // reset iterator
        filingReportsIt.resetToStart();

        while( (report = filingReportsIt.nextTag()) != NULL)
        {
            XmlElement* nameNode = report->getFirstChild(tagName);
            reportName = nameNode->mytext();

            LOG_INFO << "\n Handling report named: " << reportName << "\n";

            if (!foundIncomeRep && boost::regex_search(reportName, income_pattern))
            {
                // continue of treating "Parenthetical"

                foundIncomeRep = true;
                LOG_INFO << "FOUND INCOME REPORT MATCH!\n"<< reportName << "\n";
                reports_map->insert( pair<ReportType,string>(
                                         ReportType::INCOME,
                                         readReportHtmlNameFromRepTag(report)) );
                break;
            }
        }
    }
    if(!foundBalanceRep)
        LOG_ERROR<<"Could NOT find Balance statement\n";
    if(!foundCoverRep)
        LOG_ERROR<<"Could NOT find Cover statement\n";
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
    auto reports = new map<ReportType,string>;

    // Parse by xml
    getReportDocNames(filingSummary,reports);

    // Parse by string tokenizer
    //Tokenizer filingSummaryTok(filingSummary);
   // filingSummaryTok.getReportDocNames(reports);

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
        if (docFileString == "")
            LOG_ERROR << "Failed to retrive (COVER REPORT) named "<< reportKey << " from filing";
        else
            extracted_reports->insert( pair<ReportType,string>(
                                           reportType,
                                           docFileString) );
    }

}

string
Parser::get_report_from_complete_filing(string& page, ReportType reportType )
{
    Tokenizer tokenizer(page);
    //cout << "\n Called extract_quarterly_income() with downladed doc " 
    //     << page.substr(0,300) << endl;
    string filingSummary = tokenizer.findFilingSummary();

    if (filingSummary == "")
    {
        LOG_ERROR << "Cannot procede to handle without filing summary";
        return "";
    }

    Tokenizer filingSummaryTok(filingSummary);
    auto reports = new map<ReportType,string>;
    filingSummaryTok.getReportDocNames(reports);

    string retRep("");
    string reportKey;
    if ( (reports->find(reportType)) != reports->end() )
    {
        reportKey = reports->find(reportType)->second;
        string docString = "<FILENAME>"+reportKey;
        retRep = tokenizer.findDoc(docString);
        LOG_INFO << "\n Found report " << reportKey ;
    }
    return retRep;
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
get_units_from_text(string& text, bool share_data = false)
{
    boost::regex sd_pattern ("(shares|share data) In", boost::regex::icase);
    boost::regex t_pattern ("In Thousands", boost::regex::icase);
    boost::regex m_pattern ("In Millions", boost::regex::icase);
    boost::regex b_pattern ("In Billions", boost::regex::icase);

    if ( (!share_data) &&
         (boost::regex_search(text, sd_pattern) ) ){
        t_pattern.assign("\\$ In Thousands", boost::regex::icase);
        m_pattern.assign("\\$ In Millions", boost::regex::icase);
        b_pattern.assign("\\$ In Billions", boost::regex::icase);
    }

    if (boost::regex_search(text, t_pattern) )
        return THOU;

    if (boost::regex_search(text, m_pattern) )
        return MILL;

    if (boost::regex_search(text, b_pattern) )
        return BILL;

    if (!share_data)
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

/* Search sub tree RECURSIVLY and return first node with name == 'tagName'
 * returns NULL if no such node can be found
 */
XmlElement*
XmlElement::getFirstChild(string tagName)
{
    if (_tagName == tagName)
        return this;

    if (_children.empty())
        return NULL;

    XmlElement* childres = NULL;
    for(auto it = _children.begin() ; it != _children.end() ; ++it){
       childres = (*it)->getFirstChild(tagName);
       if (childres != NULL)
           return childres;
    }
    // No such child node
    return NULL;
}

/* If XnlElemnt is a 'th' element, returns the colspan attribute value (the number of columns it spans)
 * returns 1 for all other cases
 */
size_t
XmlElement::span_count(string span_type){

    size_t colspan = 1;
    regex span_pattern( span_type + "span = (\\d+)");
    boost::smatch match;
    string attrs = attrText();
    if (boost::regex_search(attrs, match, span_pattern) )
    {
        //LOG_INFO << "Found colspan match mathc[0] is| "<< match[0] << " and match[1] is "<< match[1]<< "\n";
        string val = match.str(1);
        colspan = stoi(val);
    }
    return colspan;
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

    if (val.at(0) == '0'){
        LOG_INFO << "Removing leading zero from " << val;
        val = val.substr(1);
        LOG_INFO << "Now have value of " << val;

    }
    return val + adjusted_units;
}

void
writeRevenueToEarnings(O_Ep& ep, string& val, string& units)
{
    // use adjust for decimals
    LOG_INFO << "DEBUG val is "<< val << " and existing rev is "<<ep._revenue();
    if ( ep._revenue() == "")
    {
        ep._revenue() = adjustForDecimals(val,units);
        return;
    }
    // Banks have total income on TWO lines that need to be added together
    long sub_rev = stol(ep._revenue());
    string new_val = adjustForDecimals(val,units);
    LOG_INFO << "DEBUG new_val is "<< new_val;
    long total = sub_rev + stol( new_val );
    LOG_INFO << "DEBUG total val is "<< to_string(total);
    ep._revenue() = to_string(total);
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
        XmlElement* tdnode = tdIt.at(_col_num,_exact_col_match);
        if (tdnode == NULL){
            LOG_ERROR << "td child at "<< _col_num << " is NULL";
            return false;
        }
        string tdtext = tdnode->text();

        LOG_INFO << "Matching val from text: "<<tdtext;

        regex not_a_value("&#160;");
        if (regex_search(tdtext,not_a_value)){
            LOG_INFO << "Matched text to 'not a value' string " << not_a_value.str() << " , so NO GOOD \n";
            return false;
        }

        if (boost::regex_search(tdtext, match, extract_pattern) )
        {       
            string val = match[0];
            LOG_INFO << "\n extracted val is"<< val;
            func(earnings,val,units);
            return true;
        } else {
            regex tiny_digit = extract_pattern;
            tdtext = removeleadingComma(tdtext);
            if (func == writeIncomeToEarnings){
                // remove any addional text that may be meta-text about figure
                size_t endpoint = tdtext.find(":");
                if (endpoint != string::npos){
                    tdtext = tdtext.substr(0,endpoint);
                    LOG_INFO << "String tdtext shortend to"<<tdtext;
                }
                tiny_digit.assign("\\(?\\d\\.?\\d?\\)?");
            }
            if (func == writeEpsToEarnings){
                // allow for no dicimal point
                tiny_digit.assign("\\(?\\d+\\)?");
                if (boost::regex_search(tdtext, match, tiny_digit) )
                {
                    string val = match[0];
                    LOG_INFO << "\n extracted val for EPS is"<< val;
                    func(earnings,val,units);
                    return true;
                }
            } // end special handling for eps
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

    boost::regex sd_pattern ("(shares|share data) in (thousands|millions|billions)", boost::regex::icase);
    boost::smatch match;
    if (boost::regex_search(titleText, match, sd_pattern) )
    {
        string units_text = match[0];
        units = get_units_from_text( units_text, true );
        LOG_INFO << "Found units in title text, they are "<< units;
    } else {

        size_t split = titleText.find("except");
        if (split != string::npos)
        {
            string additional = titleText.substr(split, titleText.size()-split);
            string pretext = titleText.substr(0, split);
            LOG_INFO << "Checking for share units in title text ";
            boost::regex a_pattern ("share data", boost::regex::icase);

            if (boost::regex_search(additional, a_pattern) )
            {
                units = get_units_from_text( additional, true );
                LOG_INFO << "Found units in title text, they are "<< units;
            } else {
                if (boost::regex_search(pretext, a_pattern) )
                {
                    units = get_units_from_text( pretext, true );
                    LOG_INFO << "Found units in title text, they are "<< units;
                }
            }
        }
    }
    LOG_INFO << "Returned unist for shares in title text are"<< units;
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
    // there may be multiple lines for long term debt
    // we ADD together each one that was found
    long new_debt = stol( adjustForDecimals(val,units) );
    long existing_debt = 0;
    if (bs._long_term_debt() != "")
        existing_debt = stol( bs._long_term_debt());
    string ltds = to_string(existing_debt + new_debt);
    LOG_INFO << "Adding to long term debt of "<< bs._long_term_debt()
                << " new value for total of "<< ltds;
    bs._long_term_debt() = ltds;
}
void
writeBookValueBalance(O_BalanceSheet& bs, string& val, string& units)
{
    LOG_INFO << "Book value for  "<<val<<"\n";

    // Handle negative values
    string sign("");
    if (val[0] == '(')
    {
        val = removeParenthasis(val);
        sign = "-";
    }
    // use adjust for decimals
    bs._book_value() = sign + adjustForDecimals(val,units);
}

size_t 
Parser::findColumnToExtract(XmlElement* tree, size_t year, size_t quarter)
{
    string titleText = get_title_text(tree);
    regex date_pattern("(\\w\\w\\w).? (\\d+)?, (\\d+)?");

// calculate relavent end date from - stock.fye, ep.year, ep.quarter
    date end_date = calculateEndDate(_stock._fiscal_year_end(), _stock._fy_same_as_ed(), year, quarter);
    LOG_INFO << " End date calucualted for year " << year << " Q| " << quarter << " from fyend of "
             << _stock._fiscal_year_end() << " and fd_same_as_ed of " <<_stock._fy_same_as_ed()
             << " is: " << end_date;

    // new addition
    size_t* col_to_ex = new size_t(1);
  //  bool* exact_match = new bool(false);
    find_data_column(tree,end_date,col_to_ex, &_exact_col_match, (quarter > 0) );
    return *col_to_ex;

/* Below is deprecated
    size_t* col_range_start = new size_t(1), *col_range_end = new size_t(1);
    bool found_col_range(false);
    if (quarter > 0)
        found_col_range = find_columns_range(tree, col_range_start, col_range_end);

    sregex_iterator mit(titleText.begin(), titleText.end(), date_pattern);
    sregex_iterator mEnd;
    size_t col_num = 1;
    bool foundDate = false;
    for(size_t i = 0; mit != mEnd; ++mit)
    {
        ++i;
        string dateStr = (*mit)[0].str();
        date rep_date = convertFromDocString(dateStr);
        // cout << "\n Examining date: "<< to_simple_string(rep_date)<<endl;
        if (foundDate)
        {
            if ( found_col_range && (i > *col_range_end) ){
                LOG_ERROR << " Still searching for column to extract, but reached end of found range"
                             << ". col_num is set as " << col_num;
                return col_num;
            }
            if ( ( rep_date == (end_date - years(1)) ) &&
                 ( col_num == i-1 ) )
            {
                if ( found_col_range && (col_num < *col_range_start) ){
                    LOG_INFO << " Found column for correct date, but it is not in the range found for time period";
                    continue;
                }
                LOG_INFO<<"Setting column index to "<<to_string(col_num);
                break;
            }
        }
        if (rep_date == end_date)
        {
            LOG_INFO << "Date matches the requested report date, col num is "<< i ;
            col_num = i;
            foundDate = true;
        }
    }
    return col_num; */
}

bool
Parser::findDefref(trIterator& trIt, regex& defref, regex& num_pattern, string& units,
           DMMM::O_Ep& earnings_data, void(*func)(O_Ep&,string&,string&), string stop_search)
{
    trIt.resetToStart();
    XmlElement* trp;
    regex stop_pattern(stop_search);
    bool founddefref = false;
    while( (trp = trIt.nextTr()) != NULL )
    {
        string attr_text = trp->attrText();
        string full_text = trp->text();
        if (stop_pattern.str().length() > 3)
            if (regex_search( full_text, stop_pattern))
                break;
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


    regex defref("defref_us-gaap_Revenues'");
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

     if (( _stock._company_type() == EnumStockCOMPANY_TYPE::COMPANY) ||
         ( _stock._company_type() == EnumStockCOMPANY_TYPE::FINANCE) )
     {
         LOG_INFO << "Going to see if Handling "<<_stock._ticker()<< " as a BANK";

         // Interest Income
         defref.assign("us-gaap_InterestAndDividendIncomeOperating'");
         if (( foundRev = findDefref(trIt, defref, num_pattern, units,
                                     earnings_data, writeRevenueToEarnings ))){
             LOG_INFO<<" Successfully found REVENUE using us-gaap_InterestAndDividendIncomeOperating (BANK specific)";
         }
         // NON-interest income
         defref.assign("us-gaap_NoninterestIncome'");
         if (( foundRev = findDefref(trIt, defref, num_pattern, units,
                                     earnings_data, writeRevenueToEarnings )))
         {
             LOG_INFO<<" Successfully found REVENUE using us-gaap_Interest(\\w*)IncomeOperating (BANK specific)";
         }
         if ( _stock._company_type() == EnumStockCOMPANY_TYPE::FINANCE ){
             if (foundRev)
             {
                 return true;
             } else {
                 // add Note;
                 // return
             }
         }
         if ( ( _stock._company_type() == EnumStockCOMPANY_TYPE::COMPANY)
              && foundRev )
         {
             LOG_INFO << "Found intrest income for "<<_stock._ticker()<< ". Going to mark it as a BANK";
             _stock._company_type() = EnumStockCOMPANY_TYPE::FINANCE;
             _stock.update();
             return true;
         }
     }// Banks


    // If compnay is a REIT - they report interest income
    // e.g. NLY, ANH
    if ( _stock._company_type() == EnumStockCOMPANY_TYPE::REIT)
    {
        LOG_INFO << "Handling "<<_stock._ticker()<< " as a REIT";

        defref.assign("us-gaap_Interest\\w*IncomeOperating|us-gaap_InterestIncomeExpenseNet");
        if (( foundRev = findDefref(trIt, defref, num_pattern, units,
                                    earnings_data, writeRevenueToEarnings )))
        {
            LOG_INFO<<" Successfully found REVENUE using us-gaap_Interest(\\w*)IncomeOperating (REIT specific)";
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
    regex num_pattern("\\(?\\d+[,\\d]*(.\\d)?\\)?");

    //NOTE: there can be multiple net income lines, due to subdivitions (e.g. SO, F).
    //Parent company should appear first, subdivitions are marked as "[Member]" and used to stop search
    string stop_string("\\[Member\\]");

    // this defref include cases of 'attributable to'
    // Note - that there is a ' terminating the string.
    // However, there may be multiple such lines in the document
    regex defref("defref_us-gaap_NetIncomeLoss'");
    if (( foundInc = findDefref(trIt, defref, num_pattern, units,
                                earnings_data, writeIncomeToEarnings, stop_string )))
    {
        LOG_INFO<<" Successfully found NET INCOME using defref defref_us-gaap_NetIncomeLoss' (1st)";
        return foundInc;
    }

    // CAT
    defref.assign("us-gaap_NetIncomeLossAvailableToCommonStockholders");
    if (( foundInc = findDefref(trIt, defref, num_pattern, units,
                                earnings_data, writeIncomeToEarnings, stop_string  )))
    {
        LOG_INFO<<" Successfully found NET INCOME using defref defref_NetIncomeLossAvailableToCommonStockholders (2nd)";
        return foundInc;
    }

    // CAR
    defref.assign("us-gaap_ComprehensiveIncomeNetOfTax");
    if (( foundInc = findDefref(trIt, defref, num_pattern, units,
                                earnings_data, writeIncomeToEarnings, stop_string )))
    {
        LOG_INFO<<" Successfully found NET INCOME using defref defref us-gaap_ComprehensiveIncomeNetOfTax (3rd) \n"
        << "It is "<< earnings_data._net_income();
        return foundInc;
    }

    // BSX
    defref.assign("us-gaap_ProfitLoss");
    if (( foundInc = findDefref(trIt, defref, num_pattern, units,
                                earnings_data, writeIncomeToEarnings, stop_string )))
    {
        LOG_INFO<<" Successfully found NET INCOME using defref defref us-gaap_ProfitLoss (4th)";
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
    regex digit_pattern("\\(?(\\d+)(.\\d+)\\)?");
    regex date_pattern("(\\w\\w\\w). (\\d+)?, (\\d+)?");

    //NOTE: EPS usualy appears once per statemnt, unlike net income which may appear multiple times for subdivisions
    // As such, do not limit search for EPS, as sometimes it appears after other subdivitions (but relates to parent)

    //LOG_INFO << "tree is \n"<<tree->printXmlTree(0);
    LOG_INFO << "extractEps() called";

    regex defref("EarningsPerShareDiluted'");
    if (( foundEps = findDefref(trIt, defref, digit_pattern, units,
                                earnings_data, writeEpsToEarnings )))
    {
        LOG_INFO<<" Successfully found EPS using defref_us_EarningsPerShareDiluted (1st)";
        return foundEps;
    }

    // ALR
    defref.assign("us-gaap_EarningsPerShareBasicAndDiluted'");
    if (( foundEps = findDefref(trIt, defref, digit_pattern, units,
                                earnings_data, writeEpsToEarnings )))
    {
        LOG_INFO<<" Successfully found EPS using us-gaap_EarningsPerShareBasicAndDiluted (2nd)";
        return foundEps;
    }

    // BRK.A
    defref.assign("us-gaap_EarningsPerShareBasic'");
    if (( foundEps = findDefref(trIt, defref, digit_pattern, units,
                                earnings_data, writeEpsToEarnings )))
    {
        LOG_INFO<<" Successfully found EPS using us-gaap_EarningsPerShareBasic (3rd)";
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

    // e.g. NS
    if (_stock._company_type() == EnumStockCOMPANY_TYPE::PIPELINE)
    {
        LOG_INFO << "Handling "<<_stock._ticker()<< " PIPLINE";
        defref.assign("us-gaap_NetIncomeLossPerOutstandingLimitedPartnershipUnitBasicNetOfTax");
        if (( foundEps = findDefref(trIt, defref, digit_pattern, units,
                                    earnings_data, writeEpsToEarnings )))
        {
            LOG_INFO<<" Successfully found EPS (or EPU) using us-gaap_NetIncomeLossPerOutstandingLimitedPartnershipUnitBasicNetOfTax" <<
                      " (PIPLINE specific)";
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
            if(nsrUnits == "")
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

    // FOR NS
    defref.assign("us-gaap_WeightedAverageLimitedPartnershipUnitsOutstanding");
    while( (trp = trIt.nextTr()) != NULL )
    {
        string attr_text = trp->attrText();
        LOG_INFO << "Attr text is: "<<attr_text;
        if ( regex_search( attr_text, defref ) )
        {
            // check for units in trtext
            if(nsrUnits == "")
            {
                string trtext = trp->text();
                nsrUnits = checkForShareUnits(trtext);
            }
            string bothUnits = nsrUnits+"|"+units;
            if ((foundNsr = checkTrPattern(attr_text, defref, bothUnits, trp,
                                           num_pattern, earnings_data, writeNsToEarnings)))
            {
                LOG_INFO<<" Found UNITS using defref us-gaap_WeightedAverageLimitedPartnershipUnitsOutstanding";
                return true;
            }
        } // if defref regex match
    } // while defref #2

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

XmlElement*
Parser::convertReportToTree(string& report)
{
    string tableStr = extractFirstTableStr(report);
    XmlElement* tree = buildXmlTree(tableStr);
    return tree;
}

bool
Parser::getNumSharesFromCoverReport(string& report, O_Ep& ep)
{
    if (report ==""){
        LOG_ERROR << "No cover report available. So exusted attempt to "
                  << "read num shares. Setting them to 0";
        ep._shares() = "0";
        return false;
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
            units = get_units_from_text( additional, true );
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
    size_t counter = 0;
    while( (trp = trIt.nextTr()) != NULL )
    {
        string trtext = trp->text();

        if (regex_search(trtext,shares_pattern))
        {
            //iterate over tds
            boost::regex pattern("\\d[,\\d]+(\\.)?\\d");
            boost::regex exclude_pattern("\\&#160;");

            tdIterator tdIt(trp);
            XmlElement* td = trp;

            // Some stocks (e.g. Google) have multiple classes of stock, only taking data for the first one
            // Some cover reports (e.g. AEE) include sub units, stop searching if this is the case
            while( ( (td = tdIt.nextTag() ) != NULL ) &&
                   (numshares == "") )
            {
                if (boost::regex_search(td->text(), exclude_pattern) )
                    continue;

                //initialzie, since this object has itererators that become invlaid otherwise
                boost::smatch match;
                if (boost::regex_search(td->text(), match, pattern) ){
                    numshares = match.str(0);

                    LOG_INFO << "Got num shares from cover report text "<< td->text() << " \n"
                             << "matche[0] is: "<< match.str(0) << " \n"
                             << "Saved to string as " << numshares << " \n";
                }

            }// itereration over tds
            ++counter;
        }
    } // iteration over trs

    if (counter > 1)
    {
        cout << "! There are multiple classes of shares\n";
        O_Note note;
        note._stock_id() = _stock._id();
        note._year() = ep._year();
        note._pertains_to() = EnumNotePERTAINS_TO::SHARES_OUTSTANDING ;
        note._note() = "There are MULTIPLE classes of shares for thes stock";
        note.insert();

    }

    if (numshares == "")
    {
        LOG_ERROR << "Failed to retrieve num shares from cover report";
        return false;
    }
    // Need to add the "|" for units|bunits structure
    units = "|"+units;
    ep._shares_diluted() = false;
    writeNsToEarnings( ep, numshares, units);
    return true;
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
    if (_col_num < 0)
        return;

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

    /* Where to get numshare data, by order of prefrence:
     * 1) income statment 'diluted'
     * 2) Cover report - outstanding
     * 3) income statment, 'basic' - NOT implememted yet
     */
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
        XmlElement* tdnode = tdIt.at(_col_num,_exact_col_match);
        if (tdnode == NULL){
            LOG_ERROR << "td child at "<< _col_num << " is NULL";
            return false;
        }
        string tdtext = tdnode->text();

        LOG_INFO << "Matching val from text: "<<tdtext;
        if (boost::regex_search(tdtext, match, extract_pattern) )
        {
            string val = match.str(0);
            LOG_INFO << "\n extracted val is"<< val;
            func(balance,val,units);
            return true;
        } else {

            regex tiny_digit = extract_pattern;
            tdtext = removeleadingComma(tdtext);
            if (func == writeCurrentAssetsToBalance )
                tiny_digit.assign("\\(?\\d\\.?\\d?\\)?");
            // this seems to do nothing...
        }
    }
    return false;
}

bool
Parser::findDefref(trIterator& trIt, regex& defref, regex& num_pattern, string& units,
           DMMM::O_BalanceSheet& balance_data, void(*func)(O_BalanceSheet&,string&,string&),
                   string stop_search)
{
    LOG_INFO << "Looking for defref "<<defref.str()<<" stop pattern is: "<< stop_search;
    regex stop_pattern(stop_search);
    trIt.resetToStart();
    XmlElement* trp;
    bool founddefref = false;
    while( (trp = trIt.nextTr()) != NULL )
    {
        string attr_text = trp->attrText();
        if (stop_pattern.str().length() > 3)
            if (regex_search( attr_text, stop_pattern))
                break;
        // LOG_INFO<<"attr_text is: "<<attr_text;
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
    string stop_search = "us-gaap_Liabilities'";
    if (( foundCA = findDefref(trIt, defref, num_pattern, units,
                               balance_data, writeCurrentAssetsToBalance,
                               stop_search)))
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
    regex stop_pattern("total liabilities", regex::icase);
    while( (trp = trIt.nextTr()) != NULL )
    {
        string trtext = trp->text();
        LOG_INFO << "\n Handling line - \n" << trtext;
        if (regex_search(trtext, stop_pattern))
            break;
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
    string stop_search = "us-gaap_Liabilities'";
    // **** Search for CL using 'defref' html attribute
    regex defref("us-gaap_LiabilitiesCurrent");
    if (( foundCL = findDefref(trIt, defref, num_pattern, units,
                               balance_data, writeCurrentLiabilitiesToBalance,
                               stop_search)))
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
    regex stop_pattern("total liabilities", regex::icase);
    while( (trp = trIt.nextTr()) != NULL )
    {
        string trtext = trp->text();
        LOG_INFO << "\n Handling line - \n" << trtext;
        if (regex_search(trtext, stop_pattern))
            break;

        boost::regex cl_pattern("total current liabilities", boost::regex::icase );
        if ( regex_search( trtext, cl_pattern)) {
            foundCL = checkTrPattern( trtext, cl_pattern, units, trp,
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
        LOG_INFO << "Looking for TOTAL LIABILITIES by block strocture";
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
                regex tl_pattern("^((\\s|:)*total liabilities)", regex::icase );

                // looking for EXACT match here
                tdIterator tdIt(trp);
                string row_title = tdIt.at(0)->text();
                   LOG_INFO<<"row title is |"<<row_title<<"|";
                if (regex_match(row_title,tl_pattern))
                    if ((foundTL = checkTrPattern( trtext, tl_pattern, units, trp,
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
    regex num_pattern("\\(?\\d+[,\\d]+(.\\d+)?\\)?");

    // **** Search for BV using 'defref' html attribute
    regex defref("us-gaap_StockholdersEquity'");
    if (( foundBV = findDefref(trIt, defref, num_pattern, units,
                                balance_data, writeBookValueBalance )))
    {
        LOG_INFO<<" Successfully found Book Value using us-gaap_StockholdersEquity (1st)";
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

    LOG_INFO << "Could not find Book value using defref. Going to use heuristics" ;

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
    // For Long Term Debt, there may be multiple lines, so we sum them all up
    // ** POSSIBLE ALTERMATIVE:
    // Use one defref regex with ALL below regex values as OR ( | | )
    // And cycle over balance sheet ONCE summing as we go
    // This will sum up all subsidiary debt if appears (as in BRK)
    regex defref("us-gaap_LongTermDebtNoncurrent");
    if (( foundLTD = findDefref(trIt, defref, num_pattern, units,
                                balance_data, writeLongTermDebtToBalance )))
        LOG_INFO<<" Successfully found Long Term Debt using us-gaap_LongTermDebtNoncurrent (1st)";

    //seniour long term notes
    // So far ONLY BERKSHIRE usese the following 3 defrefs, and does NOT use the first one
    defref.assign("us-gaap_Senior*LongTermNotes");
    if (( foundLTD |= findDefref(trIt, defref, num_pattern, units,
                                balance_data, writeLongTermDebtToBalance )))
        LOG_INFO<<" Successfully found Long Term Debt using us-gaap_Senior*LongTermNotes (2nd)";

    // junior long term notes
    defref.assign("us-gaap_Junior*LongTermNotes");
    if (( foundLTD |= findDefref(trIt, defref, num_pattern, units,
                                balance_data, writeLongTermDebtToBalance )))
        LOG_INFO<<" Successfully found Long Term Debt using us-gaap_Junior*LongTermNotes (3rd)";

    // Used by F,NSC uses us-gaap_LongTermDebtAndCapitalLeaseObligations
    defref.assign("us-gaap_(LongTerm)?DebtAndCapitalLeaseObligations");
    if (( foundLTD |= findDefref(trIt, defref, num_pattern, units,
                                balance_data, writeLongTermDebtToBalance )))
        LOG_INFO<<" Successfully found Long Term Debt us-gaap_DebtAndCapitalLeaseObligations (4th)";

    // Used by EA uses us-gaap_ConvertibleLongTermNotesPayable
    defref.assign("us-gaap_ConvertibleLongTermNotesPayable");
    if (( foundLTD |= findDefref(trIt, defref, num_pattern, units,
                                balance_data, writeLongTermDebtToBalance )))
        LOG_INFO<<" Successfully found Long Term Debt us-gaap_ConvertibleLongTermNotesPayable (5th)";


    // "other" long term debt
    defref.assign("us-gaap_OtherLongTermDebtNoncurrent");
    if (( foundLTD |= findDefref(trIt, defref, num_pattern, units,
                                balance_data, writeLongTermDebtToBalance )))
        LOG_INFO<<" Successfully found Long Term Debt using us-gaap_OtherTermDebtNoncurrent (last)";

    if (foundLTD)
    {
        LOG_INFO << "Found Long Term Debt using defref for TOTAL if "<< balance_data._long_term_debt();
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
    if(_col_num < 0)
        return;
    LOG_INFO << "Extractino col num for balance sheets is " << _col_num;


    bool has_ca = extractCurrentAssets(tree, balance_data, units);
    bool has_ta = extractTotalAssets(tree, balance_data, units);
    bool has_cl = extractCurrentLiabilities(tree, balance_data, units);
    bool has_tl = extractTotalLiabilities(tree, balance_data, units);
    extractLongTermDebt(tree, balance_data, units);
    if (has_ca && has_cl)
        _stock._has_currant_ratio() = true;
    else
        _stock._has_currant_ratio() = false;
    _stock.update();

    bool has_bv = extractBookValue(tree, balance_data, units);

    if (!has_tl && has_bv && has_ta)
        calculate_total_liabilities(balance_data);

    if (!has_bv && has_ta && has_tl)
        calculate_book_value(balance_data);
}

void
Parser::extractFiscalDatesFromReport(string& report, int* focus_year, string* date_end, int* year_end)
{
    LOG_INFO << "going to extract from cover report: (1) focus year (2) document end date (3) fiscal year end date\n";
    XmlElement* tree = NULL;
    try{
        tree = convertReportToTree(report);
    }  catch (std::exception& e) {
        LOG_ERROR << "Could not parse cover report into tree. Cannot extract fiscal dates";
        return;
    }
    trIterator trIt(tree);
    XmlElement* trp = tree;

    string fiscal_end_date(""), fiscal_year_for("");
    //int year = -1;
    regex end_pattern("Fiscal Year End Date", regex::icase);
    regex focus_pattern("Document fiscal year focus", regex::icase);
    regex doc_end_pattern("Document period end date", regex::icase);

    while( (trp = trIt.nextTr()) != NULL )
    {
        string trtext = trp->text();
        if (regex_search(trtext, end_pattern))
        {
            boost::regex ed_pattern("\\d\\d-\\d\\d");
            boost::smatch match;
            if (boost::regex_search(trtext, match, ed_pattern) )
            {
                fiscal_end_date = match[0];
                LOG_INFO << "fiscal year ends on "<< fiscal_end_date;
                //boost::shared_ptr<string> f(new Foo);
                *date_end = fiscal_end_date;
                LOG_INFO << "date_end is now" << *date_end;
            } else
                LOG_ERROR << "Could not get fiscal year end date (mm/dd)";
        }
        if (regex_search(trtext, focus_pattern))
        {
            boost::regex year_pattern("\\d\\d\\d\\d");
            boost::smatch match;
            if (boost::regex_search(trtext, match, year_pattern) )
            {
                fiscal_year_for = match[0];
                LOG_INFO << "Report is for "<< fiscal_year_for;
                *focus_year = stoi(fiscal_year_for);
            } else
                LOG_ERROR << "Could not get the focus year";
        }
        if ((regex_search(trtext, doc_end_pattern)) && (year_end != NULL))
        {
            boost::regex year_pattern("\\d\\d\\d\\d");
            boost::smatch match;
            if (boost::regex_search(trtext, match, year_pattern) )
            {
                fiscal_year_for = match[0];
                LOG_INFO << "Report period end date is YEAR only"<< fiscal_year_for;
                *year_end = stoi(fiscal_year_for);
            } else
                LOG_ERROR << "Could not get report period end date";
        }
    }
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

    Acn* acn_rec = new Acn( acn, report_date);
    LOG_INFO << "Going to calculate year and quarter for report from "<< report_date
             << " for stock "<< _stock._ticker() << "with fyed " << _stock._fiscal_year_end();

    if (!acn_rec->setAcnYearAndQuarter(report_date, _stock._fiscal_year_end()) )
    {
        LOG_ERROR << "FAILED to calculate year and quarter for ACN no "
                  << acn_rec->_acn << " and dated " << acn_rec->_report_date;
    }
    // For some stocks, if fiscal year end date is early in the year
    // the fiscal focus year is the previous year
    if ( !_stock._fy_same_as_ed() )
        acn_rec->_year = (acn_rec->_year - 1);

    LOG_INFO << "Extracted ACN: "<< acn << " filed on date" << report_date
                << " Evaluated it to be for year: " << acn_rec->_year << " adn quarter: " << acn_rec->_quarter;
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

