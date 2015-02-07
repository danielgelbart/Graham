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
    
    string text();
    void printXmlTree(size_t depth);

    XmlElement* firstNodebyName(string& tagName);
    XmlElement* tagWithText(string& tagName, string& phrase, 
                            const size_t, size_t* counter);
    void getNodes(string tagName, //with no leading or trailng whitespace 
                  size_t number, // the number of such xml elements to collect
                  vector<XmlElement*>* collected);
    
public: //members
    string              _tagName;
    vector<XmlElement*> _children;
    map<string,string>  _attributes;
    string              _text;
}; // end class XmlElement

class trIterator {
public:
    trIterator(XmlElement* node): _i(0)
        {
            if(node == NULL){
                _node = node;
            }else{
                string table("table");
                if( node->_tagName == table ){
                    _node = node;
                }else{
                    for( auto it = node->_children.begin() ; 
                         it!= node->_children.end(); ++it )
                        if( (*it)->_tagName == table)
                        {
                            _node = *it;
                            break;
                        }
                } // else2
            }   // else1
        } // C-tor      
    
    XmlElement* nextTr();
    void resetToStart(){ _i = 0; }

private:
    //members   
    XmlElement* _node;
    size_t _i;
}; // class iterator

class tdIterator {
public:
    tdIterator(XmlElement* node){
        if(node == NULL){
            _node = node;
        }else{
            string tr("tr");
            if( node->_tagName == tr ){
                _node = node;
            }else{
                for( auto it = node->_children.begin() ; 
                     it!= node->_children.end(); ++it )
                    if( (*it)->_tagName == tr)
                    {
                        _node = *it;
                        break;
                    }
                } // else2
            }   // else1
        } // C-tor      
    XmlElement* at(size_t i);
    XmlElement* nextTd();
private:
    //members   
    XmlElement* _node;
}; // class td iterator

class Parser {
public:
    Parser():_col_num(0) {}

    XmlElement* buildXmlTree(string& xmlDocument);
    void parseXML(XmlElement* node, Tokenizer& tok);

    void extract_reports(string& k10, 
                         map<ReportType,string>* extract_reports);
    string extract_quarterly_income(string& page);
    string extractFirstTableStr(string& incomeStr);
    vector<Acn*> getAcnsFromSearchResults(string& page,
                                    size_t limit,
                                    StatementType st = StatementType::Q10);
    Acn* trToAcn( XmlElement* tr );

    void parseQuarterlyIncomeStatment(XmlElement* tree, 
                                      string& units, string& currency,
                                      string& revenue, string& income, 
                                      float& eps, string& numshares);
    string getUnitsAndCurrency(XmlElement* tree, 
                             string& units, string& currency);
    void parseIncomeTree(XmlElement* tree, DMMM::O_Ep& earnigs_data);

    vector<size_t> titleInfo(XmlElement* tree, string& units, 
                             string& currency, bool singleYear);
    vector<string> getRevenues(XmlElement* tree, bool singleYear);
    vector<string> getIncs(XmlElement* tree, bool singleYear);
    vector<float> getAnualEps(XmlElement* tree, bool singleYear);
    double getQarterEps(XmlElement* tree);
    vector<string> getNumShares(XmlElement* tree, string& bunits);
    string getNumSharesFromCoverReport(string& report);
    string extractFiscalDateFromReport(string& report);
    void set_stock(DMMM::O_Stock& stock);

private:

    XmlTokenType tokenType( string& xml);
    vector<string> getTrByName(XmlElement* tree, string& trTitlePattern,
        bool singleYear);
    XmlElement* edgarResultsTableToTree(string& page);
    size_t findColumnToExtract(XmlElement* tree, DMMM::O_Ep& earnigs_data);
    bool checkTrPattern( string& text, boost::regex& title_pattern, 
                         string& units, XmlElement* node,
                         boost::regex& extract_pattern, DMMM::O_Ep& earnings,
                         void(*func)(DMMM::O_Ep&,string&,string&));
    bool
    extractTotalRevenue(XmlElement* tree, DMMM::O_Ep& earnigs_data,
                        string& units);


// members - use to save relavent data for parsing
    DMMM::O_Stock _stock;
    size_t _col_num;
};

#endif //PARSER





