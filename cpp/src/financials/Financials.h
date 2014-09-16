#ifndef FINANCIALS_H
#define FINANCIALS_H

#include <boost/filesystem.hpp>   
#include "HttpClient.h"

using namespace boost::filesystem;


class EdgarData {
public:
    EdgarData() : 
        mHttpClient(std::string("Mozilla/5.0 (compatible; Googlebot/2.1; +http://www.google.com/bot.html)"))
        {}
    
    void updateFinancials();
    void downloadAndSave(Url& url, std::string& info, const path& writeDest);
private:

    //
    void write_to_disk(std::string& doc, 
                       std::string& info, 
                       const path& writeDest);
    
private:
    HttpClient mHttpClient;

};

#endif //FINANCIALS_H
