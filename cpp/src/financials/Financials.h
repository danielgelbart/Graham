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

    void extractFinantialStatementsToDisk(string& k10, Info& info);
    void parseStatementsToDB();

private:
    string getLastYear10KAcn(O_Stock& stock);

    void downloadAndSave10k(Url& url, Info& info);
    void downloadToString(Url& url, string& rContent);
    void downloadAndSave(Url& url, Info& info, const path& writeDest);

private:
    HttpClient mHttpClient;

};



#endif //FINANCIALS_H
