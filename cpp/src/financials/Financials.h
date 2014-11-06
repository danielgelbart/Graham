#ifndef FINANCIALS_H
#define FINANCIALS_H

#include <boost/filesystem.hpp>   
#include "HttpClient.h"
#include "info.h"

using namespace boost::filesystem;

#define FINANCIALS_PATH "../../financials"

class EdgarData {
public:
    EdgarData() : 
        mHttpClient(std::string("Mozilla/5.0 (compatible; Googlebot/2.1; +http://www.google.com/bot.html)"))
        {}
    
    void updateFinancials();
    void extractFinantialStatementsToDisk(string& k10, Info& info);

private:
    void downloadAndSave10k(Url& url, Info& info);
    void downloadToString(Url& url, Info& info, string& rContent);
    void downloadAndSave(Url& url, Info& info, const path& writeDest);

private:
    HttpClient mHttpClient;

};

#endif //FINANCIALS_H
