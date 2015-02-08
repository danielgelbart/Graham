#include <assert.h>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "Logger.h"
#include "Config.h"
#include "Utils.hpp"
#include "info.h"
#include "Parser.h"
#include "Financials.h"
#include "T_Stock.hpp"
#include "T_Ep.hpp"
#include "Test.h"

bool
db_setup()
{
    // make sure we are using test DB!!
    string dbname = DBFace::instance()->getDBName();
    assert (dbname=="graham_test");
    return true;
}

string
getMockFromDisk(string fileName)
{       
//    string fileName
    path binPath = confParam<string>("argv0");
    path filePath = binPath.remove_filename() / 
        "../../financials/mocks" / fileName;

    cout << "going to load file at " << filePath.string();
    string filing =  loadFileToString(filePath.string());
    return filing;
}

string
Test::runSingleYearTest(TestResults& tResults)
{
    LOG_INFO << "\n --- Running runSingelYearTest() ---\n";
    string testName("Test-Single year 10K retrieval: ");

    T_Stock ts;
    T_Ep te;
    O_Stock stock = ts.select( ts._ticker() == string("IBM")).front();

    tResults.setStockTickerName( stock._ticker() );

    // extract to DB
    string filing = getMockFromDisk("IBM_2013_10k");
    if ( filing == "")
    {
        tResults.addFailure(testName + "Could not load mock 10k for testing");          return tResults.getResultsSummary();
    }

    Info info( stock._ticker(), 2013, StatementType::K10);
    EdgarData edgar;
    edgar.extract10kToDisk( filing, stock, info);

    //Test results writen to DB
    if (te.select( te._stock_id() == stock._id() 
                   && te._quarter() == 0 ).empty())
    {
        tResults.addFailure(testName + "No record was added to DB");       
        return tResults.getResultsSummary();
    }
    O_Ep ibm2013 = te.select( te._stock_id() == stock._id() 
                              && te._quarter() == 0 ).front();

    if (ibm2013._year() != 2013)
        tResults.addFailure(testName + "Year should be: 2013, but is: "
                            + to_string(ibm2013._year()) );
    if (ibm2013._quarter() != 0)
        tResults.addFailure(testName + "Quarter (for 10k)  should be: 0, but is: " + to_string(ibm2013._quarter()) );

    if (ibm2013._revenue() != "99751000000")
        tResults.addFailure(testName + "Revenue should be: 99751000000, but is: " + ibm2013._revenue() );

    if (ibm2013._net_income() != "16483000000")   
        tResults.addFailure(testName + "Net Income should be: 16483000000, but is: " + ibm2013._net_income() );

    if (ibm2013._eps() != 14.94)   
        tResults.addFailure(testName + "(diluted) Eps should be: 14.94, but is: " + to_string(ibm2013._eps()) );

    if (ibm2013._shares() != "1103042156")   
        tResults.addFailure(testName + "Number of (diluted) shares should be: 1103042156, but is: " + ibm2013._shares() );
  
    // test clean up
    te.erase( te._id() == ibm2013._id());

    // CVX
    stock = ts.select( ts._ticker() == string("CVX")).front();
    tResults.setStockTickerName( stock._ticker() );
    // extract to DB
    filing = getMockFromDisk("CVX_2013_10k.txt");
    if ( filing == ""){
        tResults.addFailure(testName + "Could not load mock 10k for testing");
        return tResults.getResultsSummary();
    }
    Info info1( stock._ticker(), 2013, StatementType::K10);
    edgar.extract10kToDisk( filing, stock, info1);

    //Test results writen to DB
    if (te.select( te._stock_id() == stock._id() 
                   && te._quarter() == 0 ).empty())
    {
        tResults.addFailure(testName + "No record was added to DB");       
        return tResults.getResultsSummary();
    }
    O_Ep cvx2013 = te.select( te._stock_id() == stock._id() 
                              && te._quarter() == 0 ).front();

    if (cvx2013._year() != 2013)
        tResults.addFailure(testName + "Year should be: 2013, but is: "
                            + to_string(cvx2013._year()) );
    if (cvx2013._quarter() != 0)
        tResults.addFailure(testName + "Quarter (for 10k)  should be: 0, but is: " + to_string(cvx2013._quarter()) );

    if (cvx2013._revenue() != "228848000000")
        tResults.addFailure(testName + "Revenue should be: 228848000000, but is: " + cvx2013._revenue() );

    if (cvx2013._net_income() != "21423000000")   
        tResults.addFailure(testName + "Net Income should be: 21423000000, but is: " + cvx2013._net_income() );

    if (cvx2013._eps() != 11.09)   
        tResults.addFailure(testName + "(diluted) Eps should be: 11.09, but is: " + to_string(cvx2013._eps()) );

//    if (cvx2013._shares() != "1103042156")   
    //      tResults.addFailure(testName + "Number of shares should be: 1103042156, but is: " + cvx2013._shares() );
  
    // test clean up
    te.erase( te._id() == cvx2013._id());
  
    return tResults.getResultsSummary();
}


string
Test::runSingleQarterTest(TestResults& tResults)
{
    LOG_INFO << "\n --- Running runSingleQarterTest() ---\n";
    string testName("Test-Single Quarter 10Q retrieval: ");

    T_Stock ts;
    T_Ep te;
    O_Stock stock = ts.select( ts._ticker() == string("CVX")).front();

    string filing = getMockFromDisk("CVX_2014_1stQ.txt");
    if ( filing == "")
    {
        tResults.addFailure(testName + "Could not load mock 10q for testing");          return tResults.getResultsSummary();
    }
    EdgarData edgar;
    Acn acn( string("0000093410-14-000024"), date(2014,May,2), 1 );
    edgar.addQuarterIncomeStatmentToDB(acn,stock,filing);

    //Test results writen to DB
    if (te.select( te._stock_id() == stock._id() &&
                   te._year() == 2014 && te._quarter() == 1).empty())
    {
        tResults.addFailure(testName + "No record was added to DB");       
        return tResults.getResultsSummary();
    }

    O_Ep cvx2013Q1 = te.select( te._stock_id() == stock._id() &&
                                te._year() == 2014 &&
                                te._quarter() == 1).front() ;
 
    if (cvx2013Q1._year() != 2014)
        tResults.addFailure(testName + "Year should be: 2014, but is: "
                            + to_string(cvx2013Q1._year()) );

    if (cvx2013Q1._quarter() != 1)
        tResults.addFailure(testName + "Quarter should be: 1, but is: " + to_string(cvx2013Q1._quarter()) );

    if (cvx2013Q1._revenue() != "53265000000")
        tResults.addFailure(testName + "Revenue should be: 53265000000, but is: " + cvx2013Q1._revenue() );

    if (cvx2013Q1._net_income() != "4512000000")   
        tResults.addFailure(testName + "Net Income should be: 4512000000, but is: " + cvx2013Q1._net_income() );

    if (cvx2013Q1._eps() != 2.36)   
        tResults.addFailure(testName + "(diluted) Eps should be: 2.36, but is: " + to_string(cvx2013Q1._eps()) );

    if (cvx2013Q1._shares() != "1909424000")   
        tResults.addFailure(testName + "Number of (diluted) shares should be: 1909424000, but is: " + cvx2013Q1._shares() );

    // erase record from DB
    te.erase( te._id() == cvx2013Q1._id());
    
    return tResults.getResultsSummary();
}

string 
Test::runFourthQarterTest(TestResults& tResults)
{
    LOG_INFO << "\n --- Running runFourthQarterTest() ---\n";
    string testName("Test-Create fourth quarter: ");

    T_Stock ts;
    T_Ep te;
    O_Stock stock = ts.select( ts._ticker() == string("IBM")).front();
    string filing = getMockFromDisk("IBM_2013_10k");
    Info info( stock._ticker(), 2013, StatementType::K10);
    EdgarData edgar;
    edgar.extract10kToDisk( filing, stock, info);
    edgar.createFourthQuarter(stock, 2013);

    //Test results writen to DB
    if (te.select( te._stock_id() == stock._id() 
                   && te._quarter() == 4 ).empty())
    {
        tResults.addFailure(testName + "No record was added to DB");       
        O_Ep ibm2013mockyear = te.select( te._stock_id() == stock._id() 
                                          && te._quarter() == 0 
                                          && te._year() == 2013).front();
        te.erase( te._id() == ibm2013mockyear._id());
        return tResults.getResultsSummary();
    }

    O_Ep ibm2013 = te.select( te._stock_id() == stock._id() 
                              && te._year() == 2013
                              && te._quarter() == 4 ).front();

    if (ibm2013._year() != 2013)
        tResults.addFailure(testName + "Year should be: 2013, but is: "
                            + to_string(ibm2013._year()) );
    if (ibm2013._quarter() != 4)
        tResults.addFailure(testName + "Quarter should be: 4, but is: " + to_string(ibm2013._quarter()) );

    if (ibm2013._revenue() != "27699000000")
        tResults.addFailure(testName + "Revenue should be: 27699000000, but is: " + ibm2013._revenue() );

    if (ibm2013._net_income() != "6184000000")   
        tResults.addFailure(testName + "Net Income should be: 6184000000, but is: " + ibm2013._net_income() );

    if ( withinPercent<double>( round(ibm2013._eps()), 0.05, 5.6) )  
        tResults.addFailure(testName + "(diluted) Eps should be: 5.6, but is: " + to_string(ibm2013._eps()) );

    if (ibm2013._shares() != "1103042156")   
        tResults.addFailure(testName + "Number of (diluted) shares should be: 1103042156, but is: " + ibm2013._shares() );
  

    // test clean up
    te.erase( te._id() == ibm2013._id());
    O_Ep ibm2013mockyear = te.select( te._stock_id() == stock._id() 
                                      && te._quarter() == 0 
                                      && te._year() == 2013).front();
    te.erase( te._id() == ibm2013mockyear._id());
    
    return tResults.getResultsSummary();

}

void 
Test::run_all()
{
    bool rok(false);
    rok = db_setup();
    if(!rok)
    {
        LOG_ERROR << "Something wrong with TEST(probably with TEST DB setup)."
                  << "EXITING";
        cout << "An error uccored. exiting";
        exit(-1);
    }
    string resultSummary;
    TestResults testRes;

    // test single year
    runSingleYearTest(testRes);
    runSingleQarterTest(testRes);
//    runFourthQarterTest(testRes);

    resultSummary = testRes.getResultsSummary();
    cout << "\n ---  TEST Results  ---" <<resultSummary << endl;
}

void 
Test::runCompanyTest(string& ticker)
{
    // get record from REAL DB for 2013
    LOG_INFO << "\n --- Running Test for retreval of " << ticker 
             << "  ---\n";

    cout << "\n Switching to REAL DB--------------------" << endl;

    string realDB("graham_dev");
    string testDB("graham_test");
    if ( DBFace::instance()->switchDB( realDB ) )
        cout << "Switch to real db succeccful" << endl;
    
    T_Stock rts;
    T_Ep rte;
    const O_Stock rstock = rts.select( rts._ticker() == ticker).front();
    const O_Ep real2013 = rte.select( rte._stock_id() == rstock._id() 
                               && rte._quarter() == 0
                               && rte._year() == 2013).front();
      
    cout << "\n Switching back to TEST DB--------------------" << endl;

    if (DBFace::instance()->switchDB( testDB ) )
        cout << "Switch to TEST db succeccful" << endl;
  
    // Switch back to TEST DB

    // Ensure we switched to test DB
    bool rok(false);
    rok = db_setup();
    if(!rok)
    {
        LOG_ERROR << "Something wrong with TEST(probably with TEST DB setup)."
                  << "EXITING";
        cout << "An error uccored. exiting";
        exit(-1);
    }
  
    TestResults testRes;
    T_Stock ts;
    T_Ep te;
    O_Stock stock;
    // CREATE Dummy stock in test DB!
    if ( ts.select( ts._ticker() == ticker).empty() )
    {
        stock._ticker() = ticker;
        stock._cik() = rstock._cik();
        stock.insert();

    }else{
        stock = ts.select( ts._ticker() == ticker).front();
    }
    string testName("Test-Company " + stock._ticker() + ": ");
    testRes.setTestName(testName);
    Info info( stock._ticker(), 2013, StatementType::K10);
    EdgarData edgar;
    if ( edgar.getSingleYear( stock, 2013) )
    {
        compareTest( rstock , real2013, testRes);
    }else{
        testRes.addFailure("Could Not get annual data for " + stock._ticker());
    }
    // test single year

//    runSingleYearTest(stock, real2013, testRes);

    string resultSummary = testRes.getResultsSummary();
    cout << "\n ---  TEST Results  ---" <<resultSummary << endl;
}
void
Test::seedStocks(vector<O_Stock>& stocks)
{
    string testDB("graham_test");
    cout << "\n Switching to TEST DB--------------------" << endl;

    if (DBFace::instance()->switchDB( testDB ) )
        cout << "Switch to TEST db succeccful" << endl;
    // Ensure we switched to test DB
    bool rok(false);
    rok = db_setup();
    if(!rok)
    {
        LOG_ERROR << "Something wrong with TEST(probably with TEST DB setup)."
                  << "EXITING";
        cout << "An error uccored. exiting";
        exit(-1);
    }

    T_Stock ts;
    for(auto it = stocks.begin(); it != stocks.end();++it)
    {
        if ( ts.select( ts._ticker() == it->_ticker()).empty() )
        {
            O_Stock stock;            
            stock._ticker() = it->_ticker();
            stock._cik() = it->_cik();
            stock._fiscal_year_end() = it->_fiscal_year_end();
            stock.insert();
            LOG_INFO << "Seeded " <<stock._ticker() << " to TEST DB\n";
            cout << "Seeded " <<stock._ticker() << " to TEST DB\n";
        }else{
            LOG_INFO <<it->_ticker() << "Already in TEST DB\n";
            cout <<it->_ticker() << "Already in TEST DB"<<endl;
        }
    }
}

bool 
Test::getReportsTest(O_Stock& stock, boost::filesystem::ofstream& outFile)
{
    LOG_INFO << "\n --- Testing retreval of reports for" << stock._ticker()
             << "  ---\n";
    
    // Ensure we switched to test DB
    bool rok(false);
    rok = db_setup();
    if(!rok)
    {
        LOG_ERROR << "Something wrong with TEST(probably with TEST DB setup)."
                  << "EXITING";
        cout << "An error uccored. exiting";
        exit(-1);
    }
    bool repFail(true);
    TestResults testRes;
    string testName(stock._ticker() + ": ");
    testRes.setTestName(testName);

    EdgarData edgar;
    Acn* acn = edgar.getLastYear10KAcn(stock);
    if (acn == NULL)
    {
        LOG_INFO << "Did not get acn for "<<stock._ticker() ;
        return false;
    }
    string filing = edgar.getEdgarFiling(stock,*acn);
    if (filing == "")
    {
        LOG_ERROR << "Failed to retrive filing for acn "<<acn->_acn<<"\n";
        return false;
    }

    edgar.getSingleYear(stock,2013);
    auto reports = &edgar._reports;

    LOG_INFO << "Got "<<reports->size()<<" reports\n";
    ReportType reportType = ReportType::COVER;
    auto coverReportIt = reports->find(reportType);
    if( coverReportIt == reports->end())
    {
        testRes.addFailure("NO COVER REPORT");
        repFail =false;
    }
    reportType = ReportType::INCOME;
    auto incomeReportIt = reports->find(reportType);
    if( incomeReportIt == reports->end())
    {
        testRes.addFailure("NO INCOME REPORT");
        repFail =false;
    } else {
        T_Ep te;
        O_Ep earnings_data;
        bool inDB(true);
        if (te.select( te._stock_id() == stock._id() &&
                       te._year() == 2013).empty() )
        {       
            inDB = false;
            testRes.addFailure("No earnings record for 2013 retrieved from DB");
            earnings_data = edgar._ep;
        }
        else
            earnings_data = te.select( te._stock_id() == stock._id() &&
                                            te._year() == 2013).front();

        LOG_INFO << "\n Testing values extracted for eps record with id: "
             << to_string(earnings_data._id())<<", stock_id is: "<<
            to_string(earnings_data._stock_id())<<" year "<<
            to_string(earnings_data._year()) <<", revenue: "
             <<earnings_data._revenue() << ", income: "
             <<earnings_data._net_income()<<", and eps: "<<
            to_string(earnings_data._eps())<<"\n";

        if (earnings_data._revenue() == "")
            testRes.addFailure("No Revenue exracted from Income Statement");
        if (earnings_data._net_income() == "")
            testRes.addFailure("No Income exracted from Income Statement");
        if (withinPercent(earnings_data._eps(),0.01,0.0))
            testRes.addFailure("No Eps exracted from Income Statement");
        if (earnings_data._shares() == "")
            testRes.addFailure("No Share data exracted from Income Statement");
        
        if (inDB)
            te.erase( te._id() == earnings_data._id());
    }
    
//  balancereporttest:
    reportType = ReportType::BALANCE;
    auto balanceReportIt = reports->find(reportType);
    if( balanceReportIt == reports->end())
    {
        testRes.addFailure("NO BALANCE REPORT");
        repFail =false;
    }
    string resultSummary = testRes.getResultsSummary();
    if (testRes._numFails > 0)
    {
        outFile << resultSummary;
        outFile.flush();
    }
    LOG_INFO << resultSummary;
    cout << "\n ---  TEST Results for "<<stock._ticker()<<" ---" <<resultSummary << endl;
    return repFail;
}

long
convertNumSharesToLong( string numShares )
{
    if ( numShares == removeNonDigit( numShares ) )
        return stol( numShares);

    string str = toLower(numShares);
    size_t pos = str.find("il");     
    string sufix = str.substr( pos,3 );
    string prefix = str.substr( 0, str.length()-3); 
    if ( sufix == "bil") 
        return (long)(stof(prefix) * 1000000000);
    if ( sufix == "mil" )
        return (long)(stof(prefix) * 1000000);
    return 1;
}

string 
printEarnings(const O_Ep earnings)
{
    stringstream sstr;
    sstr << " YEAR: "<<to_string( earnings._year());
    sstr << " Q: "<<to_string( earnings._quarter());
    sstr << " REVENUE: "<<earnings._revenue();
    sstr << " INCOME: "<<earnings._net_income();
    sstr << " EPS: "<<to_string( earnings._eps());
    sstr << " SHARES: "<<earnings._shares();
    return sstr.str();
}

string 
Test::compareTest(const O_Stock& rStock, const O_Ep rEarnings,
                  TestResults& tResults)
{
    LOG_INFO << "\n --- Running compareTest() ---\n";
    T_Stock ts;
    T_Ep te;
    O_Stock stock = ts.select( ts._ticker() == rStock._ticker()).front();

   //Test results writen to DB
    if (te.select( te._stock_id() == stock._id() 
                   && te._quarter() == 0 ).empty())
    {
        tResults.addFailure(string("No record was added to DB"));       
        return tResults.getResultsSummary();
    }

    // BDX fiscal year ends in Sep, so latest annual report is for 2014
// so this quesry returns empty set.
    O_Ep earnings = te.select( te._stock_id() == stock._id() &&
                               te._year() == rEarnings._year()
                               && te._quarter() == 0 ).front();

    cout << "\n Comparing DB record: "<<printEarnings(rEarnings)
      << "\n With SEC retrival: "<<printEarnings(earnings)<<endl;

    if (! withinPercent( stol(earnings._revenue()), 0.02,
                         stol(rEarnings._revenue()) ) )
        tResults.addFailure("Revenue should be: "+rEarnings._revenue()
                            +", but is: " + earnings._revenue() );
    
    if (! withinPercent( stol(earnings._net_income()), 0.02,
                         stol(rEarnings._net_income()) ) )
        tResults.addFailure("Net Income should be: "+rEarnings._net_income() 
                            +", but is: " + earnings._net_income() );
    
    if (! withinPercent( earnings._eps(), 0.02, rEarnings._eps() ) )
        tResults.addFailure("(diluted) Eps should be: "+ 
                            to_string(rEarnings._eps() )+", but is: " + 
                            to_string(earnings._eps() ) );

    long lnshares = convertNumSharesToLong( string(rEarnings._shares()) );
    LOG_INFO<<"\n Converted "<<rEarnings._shares()<<" to "
            <<to_string(lnshares)<< "\n";

    if (! withinPercent( stol(earnings._shares()), 0.02, lnshares) ) 
        tResults.addFailure("Number of (diluted) shares should be: "
                            + rEarnings._shares() 
                            + ", but is: " + earnings._shares() );
    // test clean up
    te.erase( te._id() == earnings._id());
    return tResults.getResultsSummary();
}
