#ifndef FINANCIALS_H
#define FINANCIALS_H

#include <boost/filesystem.hpp>   
#include <boost/date_time/gregorian/gregorian.hpp>
#include "HttpClient.h"

#include "O_Stock.hpp"
#include "info.h"
#include "types.h"
#include "Parser.h"



using namespace boost::filesystem;
using namespace boost::gregorian;
using namespace DMMM;

#define FINANCIALS_PATH "../../financials"


class EdgarData {
    friend class Test;
public:
    EdgarData() :
        mHttpClient(std::string("Mozilla/5.0 (compatible; Googlebot/2.1; +http://www.google.com/bot.html)"))
        {}

    bool getQuarters(O_Stock& stock);
    bool check_report_year_and_date(string cover_rep, Acn* acn_p);

    void createFourthQuarter(O_Stock& stock, size_t year);
    void createTtmEps(O_Stock& stock);

    bool getSingleYear(O_Stock& stock, size_t year);    
    bool extract10kToDisk(string& k10, O_Stock& stock, size_t year);
    void addSingleAnualIncomeStatmentToDB(string& incomeFileStr,O_Stock& stock,
                                          size_t year);
    void addSingleQuarterIncomeStatmentToDB(string& incomeFileStr, O_Stock& stock,
                                            size_t year, size_t quarter, string& cover_report);

    void addBalanceStatmentToDB(string& incomeFileStr,O_Stock& stock, size_t year);

    void updateFinancials(O_Stock& stock);
    bool getFiscalYearEndDate(O_Stock& stock);
    void loadCountryMaps();
    bool getCountry(O_Stock& stock);

private:
    string getEdgarSearchResultsPage(O_Stock& stock, StatementType st);
    string getEdgarFiling( O_Stock& stock, Acn& acn);
    Acn* getLastYear10KAcn(O_Stock& stock);

    void downloadAndSave10k(Url& url, Info& info);
    bool downloadToString(Url& url, string& rContent);
    void downloadAndSave(Url& url, Info& info, const path& writeDest);

    bool addEarningsRecordToDB( O_Stock& stock, size_t year, size_t quarter,
                            string revenue, string income, float eps,
                            string shares, const string& source);
    bool addEarningsRecordToDB( O_Stock& stock, O_Ep& incomeS);
    bool insertEp( O_Ep& ep );
    bool addBalanceRecordToDB( O_Stock& stock, O_BalanceSheet& bs);
    bool insertBs( O_BalanceSheet& bs );

private:
    HttpClient mHttpClient;
    map<ReportType,string> _reports;
    Parser _parser; // the default parser to use
    // for testing
    O_Ep _ep;
    // For selecting countries
        map<string,string> usmap;
        map<string,string> camap;
        map<string,string> omap;

};



#endif //FINANCIALS_H
