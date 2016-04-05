#ifndef PARSER_H
#define PARSER_H

#include <boost/filesystem.hpp>   
#include <boost/regex.hpp>
#include <map>
#include "O_Ep.hpp"
#include "O_Stock.hpp"
#include "types.h"
#include "Tokenizer.h"
#include "info.h"

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
    
    // returns all subtree tect concatenated by ' : '
    string text();
    // returns _text (member text only)
    string mytext();
    string attrText();
    void printXmlTree(size_t depth);

    XmlElement* firstNodebyName(string& tagName);
    XmlElement* tagWithText(string& tagName, string& phrase, 
                            const size_t, size_t* counter);
    void getNodes(string tagName, //with no leading or trailng whitespace 
                  size_t number, // the number of such xml elements to collect
                  vector<XmlElement*>* collected);
    XmlElement* getFirstChild(string tagName);
    size_t span_count(string span_type);
    
public: //members
    string              _tagName;
    vector<XmlElement*> _children;
    map<string,string>  _attributes;
    string              _text;
}; // end class XmlElement



class tagIterator {
public:
    tagIterator(XmlElement* node, string start, string tag_name);
    XmlElement* at(size_t i, bool exact_match = false);
    XmlElement* nextTag();
    void resetToStart(){ _i = 0; }
protected:
    //members
    XmlElement* _node;
    string _start;
    string _tag_name;
    size_t _i;
}; // class td iterator

// the following two classes can be removed, just need to change the code that calls them explicitly
class trIterator : public tagIterator {
public:
    trIterator(XmlElement* node): tagIterator(node,string("table"), string("tr")){}
    XmlElement* nextTr() { return tagIterator::nextTag(); }
}; // class iterator

class tdIterator : public tagIterator{
public:
    tdIterator(XmlElement* node) : tagIterator(node, string("tr"), string("td")){}
}; // class td iterator

class Parser {
public:
    Parser():_col_num(0), _exact_col_match(false){ }
    Parser(DMMM::O_Stock& stock):_col_num(0), _exact_col_match(false){ _stock = stock; }

    XmlElement* buildXmlTree(string& xmlDocument);
    void parseXML(XmlElement* node, Tokenizer& tok);

    void extract_reports(string& k10, 
                         map<ReportType,string>* extract_reports);
    string readReportHtmlNameFromRepTag(XmlElement* report);
    void getReportDocNames(string& filingSummary,map<ReportType,string>* reports);
    string get_report_from_complete_filing(string& page, ReportType reportType);
    string extractFirstTableStr(string& incomeStr);
    vector<Acn*> getAcnsFromSearchResults(string& page,
                                    size_t limit,
                                    StatementType st = StatementType::Q10);
    Acn* trToAcn( XmlElement* tr );

    string getUnitsAndCurrency(XmlElement* tree,
                             string& units, string& currency);
    void parseIncomeTree(XmlElement* tree, DMMM::O_Ep& earnigs_data, boost::gregorian::date rep_end_date);
    void parseBalanceTree(XmlElement* tree, DMMM::O_BalanceSheet& balance_data, boost::gregorian::date rep_end_date);

    vector<size_t> titleInfo(XmlElement* tree, string& units, 
                             string& currency, bool singleYear);
    double getQarterEps(XmlElement* tree);

    bool getNumSharesFromCoverReport(string& report, DMMM::O_Ep& ep);
    boost::gregorian::date extractPeriodEndDateFromCoverReport(string& report);
    void extractFiscalDatesFromReport(string& report, int* focus_year = NULL,
                                      string* date_end = NULL, int* year_end = NULL);
    void updateFiscalDates(DMMM::O_Stock& stock, int* focus_year, string* date_end, int* year_end);

    void set_stock(DMMM::O_Stock& stock);
    XmlElement* convertReportToTree(string& report);

private:

    XmlTokenType tokenType( string& xml);
    vector<string> getTrByName(XmlElement* tree, string& trTitlePattern,
        bool singleYear);
    XmlElement* edgarResultsTableToTree(string& page);

    size_t findColumnToExtract(XmlElement* tree, size_t year, size_t quarter, boost::gregorian::date rep_end_date);

    // Could use template for these methods
    bool checkTrPattern( string& text, boost::regex& title_pattern, 
                         string& units, XmlElement* node,
                         boost::regex& extract_pattern, DMMM::O_Ep& earnings,
                         void(*func)(DMMM::O_Ep&,string&,string&));
    bool checkTrPattern( string& text, boost::regex& title_pattern,
                            string& units, XmlElement* node,
                            boost::regex& extract_pattern, DMMM::O_BalanceSheet& balance,
                            void(*func)(DMMM::O_BalanceSheet&,string&,string&));

    // Could use template for these methods
    bool findDefref(trIterator& trIt, boost::regex& defref, boost::regex& num_pattern, string& units,
               DMMM::O_Ep& earnings_data, void(*func)(DMMM::O_Ep&,string&,string&),
                    string stop_search = "");
    bool findDefref(trIterator& trIt, boost::regex& defref, boost::regex& num_pattern, string& units,
               DMMM::O_BalanceSheet& balance_data, void(*func)(DMMM::O_BalanceSheet&,string&,string&),
                    string stop_search = "");

    bool extractTotalRevenue(XmlElement* tree, DMMM::O_Ep& earnigs_data,
                        string& units);
    bool extractNetIncome(XmlElement* tree, DMMM::O_Ep& earnigs_data,
                          string& units);
    bool extractEps(XmlElement* tree, DMMM::O_Ep& earnigs_data,string& units);
    string adjustValToUnits(string& val, string& units);

    bool extractNumShares(XmlElement* tree, DMMM::O_Ep& earnigs_data,
                          string& units, string& nsrUnits);

    bool extractCurrentAssets(XmlElement* tree, DMMM::O_BalanceSheet& balance_data, string& units);
    bool extractTotalAssets(XmlElement* tree, DMMM::O_BalanceSheet& balance_data, string& units);
    bool extractCurrentLiabilities(XmlElement* tree, DMMM::O_BalanceSheet& balance_data, string& units);
    bool extractTotalLiabilities(XmlElement* tree, DMMM::O_BalanceSheet& balance_data, string& units);
    bool extractLongTermDebt(XmlElement* tree, DMMM::O_BalanceSheet& balance_data, string& units);
    bool extractBookValue(XmlElement* tree, DMMM::O_BalanceSheet& balance_data, string& units);
// members - use to save relavent data for parsing
    DMMM::O_Stock _stock;
    int _col_num;
    bool _exact_col_match;

};

#endif //PARSER





