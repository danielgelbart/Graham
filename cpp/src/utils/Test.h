#ifndef TEST_H
#define TEST_H

#include <boost/filesystem.hpp>   
#include <boost/filesystem/fstream.hpp>
#include <map>

#include "types.h"
#include "Tokenizer.h"
#include "info.h"
#include "O_Stock.hpp"

using namespace std;
using namespace DMMM;
using namespace boost::filesystem;

class TestResults{
public:
    TestResults():_numFails(0), _curTestName(""), _tickerName(""){}

    void addFailure(string failMessage){
        _failMessages.push_back(_curTestName + _tickerName + failMessage);
        _numFails = _numFails + 1;
    }

    string getResultsSummary(){
        string ret("\n");

        if (_numFails == 0)
            return (ret+"Great!!! Everything is working");

        ret+="Testing shows " + to_string(_numFails) +" failures as follows:\n";
        for( size_t i=0; i<_failMessages.size();i++)
        {
            ret+=to_string(i+1)+". "+_failMessages[i] +"\n";
        }
        return ret;
    }

    void setTestName(string str){ _curTestName = str; }
    void setStockTickerName(string str){ _tickerName = str; }
public:
    size_t _numFails;
    vector<string> _failMessages;
    string _curTestName;
    string _tickerName;
}; // class TestResults

class Test {
public:
    Test(){}
    
    void run_all();
    string runSingleYearTest(TestResults& tr);
    string runSingleQarterTest(TestResults& tr);
    string runFourthQarterTest(TestResults& tr);
    void runCompanyTest(string& ticker);
    string compareTest(const O_Stock& rStock, const O_Ep rEarnings,
                       TestResults& tResults);
    void seedStocks(vector<O_Stock>& stocks);
    bool getReportsTest(O_Stock& stock,boost::filesystem::ofstream& outFile);


private: //members
//    path _mockPath;

}; //end class test
#endif
