#ifndef FINANCIALS_H
#define FINANCIALS_H

#include <boost/filesystem.hpp>   
#include <boost/date_time/gregorian/gregorian.hpp>
#include "HttpClient.h"

#include "O_Stock.hpp"
#include "info.h"

using namespace boost::filesystem;
using namespace boost::gregorian;
using namespace DMMM;

#define FINANCIALS_PATH "../../financials"


class EdgarData {
public:
    EdgarData() : 
        mHttpClient(std::string("Mozilla/5.0 (compatible; Googlebot/2.1; +http://www.google.com/bot.html)"))
        {}
    
    void updateFinancials(O_Stock& stock);
    void getQuarters(O_Stock& stock);

    void extract10kToDisk(string& k10, O_Stock& stock, Info& info);
    void addAnualIncomeStatmentToDB(string& incomeFileStr,O_Stock& stock);
    void addBalanceStatmentToDB(string& incomeFileStr,O_Stock& stock);
    void addQuarterIncomeStatmentToDB(Acn& acn, O_Stock& stock);
    void createFourthQuarter(O_Stock& stock, size_t year);

private:
    string getLastYear10KAcn(O_Stock& stock);

    void downloadAndSave10k(Url& url, Info& info);
    void downloadToString(Url& url, string& rContent);
    void downloadAndSave(Url& url, Info& info, const path& writeDest);

    bool insertEp( O_Ep& ep );

private:
    HttpClient mHttpClient;

};



#endif //FINANCIALS_H
