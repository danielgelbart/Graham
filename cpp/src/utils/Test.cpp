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
#include "T_Note.hpp"
#include "Test.h"

bool
db_setup()
{
    // make sure we are using test DB!!
    string dbname = DBFace::instance()->getDBName();
    assert (dbname=="graham_test");
    return true;
}

bool
copy_stocks()
{

    LOG_INFO << "\n --- Coping stocks table from dev environmnet ---\n";

    cout << "\n Switching to REAL DB--------------------" << endl;

    string realDB("graham_dev");
    string testDB("graham_test");
    if ( DBFace::instance()->switchDB( realDB ) )
        cout << "Switch to real db succeccful" << endl;

    T_Stock rts;
    auto stocks = rts.select();

    cout << "\n Switching back to TEST DB--------------------" << endl;

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
     // drop stocks table

    //copy in as is stocks - in block transaction?

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

bool
Test::setTestDB()
{
    cout << "\n Switching back to TEST DB--------------------" << endl;
    string testDB("graham_test");
    if (DBFace::instance()->switchDB( testDB ) )
        cout << "Switch to TEST db succeccful" << endl;
    return db_setup();
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
        tResults.addFailure(testName + "Could not load mock 10k for testing");
        return tResults.getResultsSummary();
    }

    // Hard code 2013 to be used in all stocks in test
    int year = 2013;

    EdgarData edgar;
    edgar.extract10kToDisk( filing, stock, year);

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

    //BALANCE extraction test
    T_BalanceSheet tb;
    string balanceTestName = "BalanceTest";
    O_BalanceSheet b_ibm2013 = tb.select( tb._stock_id() == stock._id()
                              && tb._year() == year ).front();

    if (b_ibm2013._year() != 2013)
        tResults.addFailure(balanceTestName + "Year should be: 2013, but is: "
                            + to_string(b_ibm2013._year()) );

    if (b_ibm2013._current_assets() != "51350000000")
        tResults.addFailure(balanceTestName + "CA should be: 51350000000, but is: " + b_ibm2013._current_assets() );
    if (b_ibm2013._total_assets() != "126223000000")
        tResults.addFailure(balanceTestName + "TA should be: 126223000000, but is: " + b_ibm2013._total_assets() );
    if (b_ibm2013._current_liabilities() != "40154000000")
        tResults.addFailure(balanceTestName + "CL should be: 40154000000, but is: " + b_ibm2013._current_liabilities() );
    if (b_ibm2013._total_liabilities() != "103294000000")
        tResults.addFailure(balanceTestName + "TA should be: 103294000000, but is: " + b_ibm2013._total_liabilities() );
    if (b_ibm2013._long_term_debt() != "32856000000")
        tResults.addFailure(balanceTestName + "LTD should be: 32856000000, but is: " + b_ibm2013._long_term_debt() );
    if (stock._has_currant_ratio() != true)
        tResults.addFailure(balanceTestName + "Should Have current ratio. but is set to FALSE" );
   // if (b_ibm2013._book_value() != "99751000000")
     //   tResults.addFailure(balanceTestName + "Calculated Book value should be: 99751000000, but is: "
       //                     + b_ibm2013._book_value() );
    // test clean up
    tb.erase( tb._id() == b_ibm2013._id());

    // CVX
    stock = ts.select( ts._ticker() == string("CVX")).front();
    tResults.setStockTickerName( stock._ticker() );
    // extract to DB
    filing = getMockFromDisk("CVX_2013_10k.txt");
    if ( filing == ""){
        tResults.addFailure(testName + "Could not load mock 10k for testing");
        return tResults.getResultsSummary();
    }

    edgar.extract10kToDisk( filing, stock, year);

    //Test results writen to DB
    if (te.select( te._stock_id() == stock._id() 
                   && te._quarter() == 0 ).empty())
    {
        tResults.addFailure(testName + "No record was added to DB");       
        return tResults.getResultsSummary();
    }
    O_Ep cvx2013 = te.select( te._stock_id() == stock._id() 
                              && te._quarter() == 0 ).front();

    if (cvx2013._year() != year)
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

    //BALANCE extraction test
    O_BalanceSheet b_cvx2013 = tb.select( tb._stock_id() == stock._id()
                              && tb._year() == year ).front();
    if (b_cvx2013._year() != 2013)
        tResults.addFailure(balanceTestName + "Year should be: 2013, but is: "
                            + to_string(b_cvx2013._year()) );

    if (b_cvx2013._current_assets() != "50250000000")
        tResults.addFailure(balanceTestName + "CA should be: 50250000000, but is: " + b_cvx2013._current_assets() );
    if (b_cvx2013._total_assets() != "253753000000")
        tResults.addFailure(balanceTestName + "TA should be: 253753000000, but is: " + b_cvx2013._total_assets() );
    if (b_cvx2013._current_liabilities() != "33018000000")
        tResults.addFailure(balanceTestName + "CL should be: 33018000000, but is: " + b_cvx2013._current_liabilities() );
    if (b_cvx2013._total_liabilities() != "103326000000")
        tResults.addFailure(balanceTestName + "TA should be: 103326000000, but is: " + b_cvx2013._total_liabilities() );
    if (b_cvx2013._long_term_debt() != "19960000000")
        tResults.addFailure(balanceTestName + "LTD should be: 19960000000, but is: " + b_cvx2013._long_term_debt() );
    if (stock._has_currant_ratio() != true)
        tResults.addFailure(balanceTestName + "Should Have current ratio. but is set to FALSE" );
    // test clean up
    tb.erase( tb._id() == b_cvx2013._id());

    // GOOG
    stock = ts.select( ts._ticker() == string("GOOG")).front();
    tResults.setStockTickerName( stock._ticker() );
    // extract to DB
    filing = getMockFromDisk("GOOG_2013_10k.txt");
    if ( filing == ""){
        tResults.addFailure(testName + "Could not load mock 10k for testing");
        return tResults.getResultsSummary();
    }

    edgar.extract10kToDisk( filing, stock, year);

    //Test results writen to DB
    if (te.select( te._stock_id() == stock._id() 
                   && te._quarter() == 0 ).empty())
    {
        tResults.addFailure(testName + "No record was added to DB");       
        return tResults.getResultsSummary();
    }
    O_Ep goog2013 = te.select( te._stock_id() == stock._id() 
                              && te._quarter() == 0 ).front();
    if (goog2013._year() != year)
        tResults.addFailure(testName + "Year should be: 2013, but is: "
                            + to_string(goog2013._year()) );
    if (goog2013._quarter() != 0)
        tResults.addFailure(testName + "Quarter (for 10k)  should be: 0, but is: " + to_string(goog2013._quarter()) );

    if (goog2013._revenue() != "59825000000")
        tResults.addFailure(testName + "Revenue should be: 59825000000, but is: " + goog2013._revenue() );

    if (goog2013._net_income() != "12920000000")   
        tResults.addFailure(testName + "Net Income should be: 12920000000, but is: " + goog2013._net_income() );

    if (goog2013._eps() != 38.13)   
        tResults.addFailure(testName + "(diluted) Eps should be: 38.13, but is: " + to_string(goog2013._eps()) );

//    if (goog2013._shares() != "1103042156")   
    //      tResults.addFailure(testName + "Number of shares should be: 1103042156, but is: " + goog2013._shares() );
  
    // test clean up
    te.erase( te._id() == goog2013._id());

    //BALANCE extraction test
    O_BalanceSheet b_goog2013 = tb.select( tb._stock_id() == stock._id()
                              && tb._year() == year ).front();
    if (b_goog2013._year() != 2013)
        tResults.addFailure(balanceTestName + "Year should be: 2013, but is: "
                            + to_string(b_goog2013._year()) );

    if (b_goog2013._current_assets() != "72886000000")
        tResults.addFailure(balanceTestName + " CA should be: 72886000000, but is: " + b_goog2013._current_assets() );
    if (b_goog2013._total_assets() != "110920000000")
        tResults.addFailure(balanceTestName + " TA should be: 110920000000, but is: " + b_goog2013._total_assets() );
    if (b_goog2013._current_liabilities() != "15908000000")
        tResults.addFailure(balanceTestName + " CL should be: 15908000000, but is: " + b_goog2013._current_liabilities() );
    if (b_goog2013._book_value() != "87309000000")
        tResults.addFailure(balanceTestName + " BV should be: 87309000000, but is: " + b_goog2013._book_value() );

    if (b_goog2013._total_liabilities() != "23611000000")
        tResults.addFailure(balanceTestName + " TL should be: 23611000000, but is: " + b_goog2013._total_liabilities() );
    if (b_goog2013._long_term_debt() != "2236000000")
        tResults.addFailure(balanceTestName + " LTD should be: 2236000000, but is: " + b_goog2013._long_term_debt() );
    if (stock._has_currant_ratio() != true)
        tResults.addFailure(balanceTestName + "Should Have current ratio. but is set to FALSE" );
    // test clean up
    tb.erase( tb._id() == b_goog2013._id());


    // BDX
    stock = ts.select( ts._ticker() == string("BDX")).front();
    tResults.setStockTickerName( stock._ticker() );
    // extract to DB
    filing = getMockFromDisk("BDX_2013_10k.txt");
    if ( filing == ""){
        tResults.addFailure(testName + "Could not load mock 10k for testing");
        return tResults.getResultsSummary();
    }

    edgar.extract10kToDisk( filing, stock, year);

    //Test results writen to DB
    if (te.select( te._stock_id() == stock._id() 
                   && te._quarter() == 0 ).empty())
    {
        tResults.addFailure(testName + "No record was added to DB");       
        return tResults.getResultsSummary();
    }
    O_Ep bdx2013 = te.select( te._stock_id() == stock._id() 
                               && te._quarter() == 0 ).front();
    if (bdx2013._year() != year)
        tResults.addFailure(testName + "Year should be: 2013, but is: "
                            + to_string(bdx2013._year()) );
    if (bdx2013._quarter() != 0)
        tResults.addFailure(testName + "Quarter (for 10k)  should be: 0, but is: " + to_string(bdx2013._quarter()) );

    if (bdx2013._revenue() != "8054000000")
        tResults.addFailure(testName + "Revenue should be: 8054000000, but is: " + bdx2013._revenue() );

    if (bdx2013._net_income() != "1293000000")   
        tResults.addFailure(testName + "Net Income should be: 1293000000, but is: " + bdx2013._net_income() );

    if (bdx2013._eps() != 6.49)   
        tResults.addFailure(testName + "(diluted) Eps should be: 6.49, but is: " + to_string(bdx2013._eps()) );

    if (bdx2013._shares() != "194094466")   
      tResults.addFailure(testName + "Number of shares should be: 194094466, but is: " + bdx2013._shares() );

    // test clean up
    te.erase( te._id() == bdx2013._id());

    //BALANCE extraction test
    O_BalanceSheet b_bdx2013 = tb.select( tb._stock_id() == stock._id()
                              && tb._year() == year ).front();

    if (b_bdx2013._current_assets() != "5873000000")
        tResults.addFailure(balanceTestName + " CA should be: 5873000000, but is: " + b_bdx2013._current_assets() );
    if (b_bdx2013._total_assets() != "12149000000")
        tResults.addFailure(balanceTestName + " TA should be: 12149000000, but is: " + b_bdx2013._total_assets() );
    if (b_bdx2013._current_liabilities() != "2130000000")
        tResults.addFailure(balanceTestName + " CL should be: 2130000000, but is: " + b_bdx2013._current_liabilities() );
    if (b_bdx2013._book_value() != "5043000000")
        tResults.addFailure(balanceTestName + " BV should be: 5043000000, but is: " + b_bdx2013._book_value() );

    if (b_bdx2013._total_liabilities() != "7106000000")
        tResults.addFailure(balanceTestName + "TL should be: 7106000000, but is: " + b_bdx2013._total_liabilities() );
    if (b_bdx2013._long_term_debt() != "3763000000")
        tResults.addFailure(balanceTestName + "LTD should be: 3763000000, but is: " + b_bdx2013._long_term_debt() );
    if (stock._has_currant_ratio() != true)
        tResults.addFailure(balanceTestName + "Should Have current ratio. but is set to FALSE" );
    // test clean up
    tb.erase( tb._id() == b_bdx2013._id());


    // BRK.A
    stock = ts.select( ts._ticker() == string("BRK.A")).front();
    tResults.setStockTickerName( stock._ticker() );
    // extract to DB
    filing = getMockFromDisk("BRK_2013_10k.txt");
    if ( filing == ""){
        tResults.addFailure(testName + "Could not load mock 10k for testing");
        return tResults.getResultsSummary();
    }
    edgar.extract10kToDisk( filing, stock, year);

    //Test results writen to DB
    if (te.select( te._stock_id() == stock._id() 
                   && te._quarter() == 0 ).empty())
    {
        tResults.addFailure(testName + "No record was added to DB");       
        return tResults.getResultsSummary();
    }
    O_Ep brk2013 = te.select( te._stock_id() == stock._id() 
                              && te._quarter() == 0 ).front();
    if (brk2013._year() != 2013)
        tResults.addFailure(testName + "Year should be: 2013, but is: "
                            + to_string(brk2013._year()) );
    if (brk2013._quarter() != 0)
        tResults.addFailure(testName + "Quarter (for 10k)  should be: 0, but is: " + to_string(brk2013._quarter()) );

    if (brk2013._revenue() != "182150000000")
        tResults.addFailure(testName + "Revenue should be: 182150000000, but is: " + brk2013._revenue() );

    if (brk2013._net_income() != "19476000000")   
        tResults.addFailure(testName + "Net Income should be: 19476000000, but is: " + brk2013._net_income() );

    if (brk2013._eps() != 11850.0)   
        tResults.addFailure(testName + "(diluted) Eps should be: 11850.0, but is: " + to_string(brk2013._eps()) );

    // test clean up
    te.erase( te._id() == brk2013._id());

    //BALANCE extraction test
    O_BalanceSheet b_brk2013 = tb.select( tb._stock_id() == stock._id()
                              && tb._year() == year ).front();

    if (b_brk2013._current_assets() != "")
        tResults.addFailure(balanceTestName + " CA should be: , but is: " + b_brk2013._current_assets() );
    if (b_brk2013._total_assets() != "484931000000")
        tResults.addFailure(balanceTestName + " TA should be: 484931000000, but is: " + b_brk2013._total_assets() );
    if (b_brk2013._current_liabilities() != "")
        tResults.addFailure(balanceTestName + " CL should be: , but is: " + b_brk2013._current_liabilities() );
    if (b_brk2013._total_liabilities() != "260446000000")
        tResults.addFailure(balanceTestName + " TL should be: 260446000000, but is: " + b_brk2013._total_liabilities() );
//    if (b_brk2013._long_term_debt() != "")
//        tResults.addFailure(balanceTestName + " LTD should be: , but is: " + b_brk2013._long_term_debt() );
    if (b_brk2013._book_value() != "221890000000")
        tResults.addFailure(balanceTestName + " BV should be: 221890000000, but is: " + b_brk2013._book_value() );
    if (stock._has_currant_ratio() == true)
        tResults.addFailure(balanceTestName + "Should NOT Have current ratio. but is set to TRUE" );
    // test clean up
    tb.erase( tb._id() == b_brk2013._id());


    // DE
    stock = ts.select( ts._ticker() == string("DE")).front();
    tResults.setStockTickerName( stock._ticker() );
    // extract to DB
    filing = getMockFromDisk("DE_2013_10k.txt");
    if ( filing == ""){
        tResults.addFailure(testName + "Could not load mock 10k for testing");
        return tResults.getResultsSummary();
    }
    edgar.extract10kToDisk( filing, stock, year);

    //Test results writen to DB
    if (te.select( te._stock_id() == stock._id() 
                   && te._quarter() == 0 ).empty())
    {
        tResults.addFailure(testName + "No record was added to DB");       
        return tResults.getResultsSummary();
    }
    O_Ep de2013 = te.select( te._stock_id() == stock._id() 
                              && te._quarter() == 0 ).front();
    if (de2013._revenue() != "37795400000")
        tResults.addFailure(testName + "Revenue should be: 37795400000, but is: " + de2013._revenue() );
    if (de2013._net_income() != "3537300000")   
        tResults.addFailure(testName + "Net Income should be: 3537300000, but is: " + de2013._net_income() );
    if (de2013._eps() != 9.09)   
        tResults.addFailure(testName + "(diluted) Eps should be: 9.09, but is: " + to_string(de2013._eps()) );
    if (de2013._shares() != "389200000")   
        tResults.addFailure(testName + "Number of shares should be: 389200000, but is: " + de2013._shares() );
    // test clean up
    te.erase( te._id() == de2013._id());

    //BALANCE extraction test
    O_BalanceSheet b_de2013 = tb.select( tb._stock_id() == stock._id()
                              && tb._year() == year ).front();
    if (b_de2013._current_assets() != "")
        tResults.addFailure(balanceTestName + " CA should be: , but is: " + b_de2013._current_assets() );
    if (b_de2013._total_assets() != "59521300000")
        tResults.addFailure(balanceTestName + " TA should be: 59521300000, but is: " + b_de2013._total_assets() );
    if (b_de2013._current_liabilities() != "")
        tResults.addFailure(balanceTestName + " CL should be: , but is: " + b_de2013._current_liabilities() );
    if (b_de2013._total_liabilities() != "49253600000")
        tResults.addFailure(balanceTestName + " TL should be: 49253600000, but is: " + b_de2013._total_liabilities() );
    if (b_de2013._long_term_debt() != "21577700000")
        tResults.addFailure(balanceTestName + " LTD should be: 21577700000, but is: " + b_de2013._long_term_debt() );
    if (b_de2013._book_value() != "10265800000")
        tResults.addFailure(balanceTestName + " BV should be: 10265800000, but is: " + b_de2013._book_value() );
    if (stock._has_currant_ratio() == true)
        tResults.addFailure(balanceTestName + "Should NOT Have current ratio. but is set to TRUE" );
    // test clean up
    tb.erase( tb._id() == b_de2013._id());


    // INTC
    stock = ts.select( ts._ticker() == string("INTC")).front();
    tResults.setStockTickerName( stock._ticker() );
    // extract to DB
    filing = getMockFromDisk("INTC_2013_10k.txt");
    if ( filing == ""){
        tResults.addFailure(testName + "Could not load mock 10k for testing");
        return tResults.getResultsSummary();
    }
    edgar.extract10kToDisk( filing, stock, year);
    //Test results writen to DB
    if (te.select( te._stock_id() == stock._id() 
                   && te._quarter() == 0 ).empty())
    {
        tResults.addFailure(testName + "No record was added to DB");       
        return tResults.getResultsSummary();
    }
    O_Ep intc2013 = te.select( te._stock_id() == stock._id() 
                             && te._quarter() == 0 ).front();
    if (intc2013._revenue() != "52708000000")
        tResults.addFailure(testName + "Revenue should be: 52708000000, but is: " + intc2013._revenue() );
    if (intc2013._net_income() != "9620000000")   
        tResults.addFailure(testName + "Net Income should be: 9620000000, but is: " + intc2013._net_income() );
    if (intc2013._eps() != 1.89)   
        tResults.addFailure(testName + "(diluted) Eps should be: 1.89, but is: " + to_string(intc2013._eps()) );
    // test clean up
    te.erase( te._id() == intc2013._id());

    //BALANCE extraction test
    O_BalanceSheet b_intc2013 = tb.select( tb._stock_id() == stock._id()
                              && tb._year() == year ).front();
    if (b_intc2013._current_assets() != "32084000000")
        tResults.addFailure(balanceTestName + " CA should be: 32084000000, but is: " + b_intc2013._current_assets() );
    if (b_intc2013._total_assets() != "92358000000")
        tResults.addFailure(balanceTestName + " TA should be: 92358000000, but is: " + b_intc2013._total_assets() );
    if (b_intc2013._current_liabilities() != "13568000000")
        tResults.addFailure(balanceTestName + " CL should be: 13568000000, but is: " + b_intc2013._current_liabilities() );
    if (b_intc2013._total_liabilities() != "34102000000")
        tResults.addFailure(balanceTestName + " TL should be: 34102000000, but is: " + b_intc2013._total_liabilities() );
    if (b_intc2013._calculated_tl() == false)
        tResults.addFailure(balanceTestName + " TL NOT suplied, so calculated_tl should be NOT be false" );
    if (b_intc2013._long_term_debt() != "13165000000")
        tResults.addFailure(balanceTestName + " LTD should be: 13165000000, but is: " + b_intc2013._long_term_debt() );
    if (b_intc2013._book_value() != "58256000000")
        tResults.addFailure(balanceTestName + " BV should be: 58256000000, but is: " + b_intc2013._book_value() );
    // test clean up
    tb.erase( tb._id() == b_intc2013._id());


    // F
    stock = ts.select( ts._ticker() == string("F")).front();
    tResults.setStockTickerName( stock._ticker() );
    // extract to DB
    filing = getMockFromDisk("F_2013_10k.txt");
    if ( filing == ""){
        tResults.addFailure(testName + "Could not load mock 10k for testing");
        return tResults.getResultsSummary();
    }
    edgar.extract10kToDisk( filing, stock, year);
    //Test results writen to DB
    if (te.select( te._stock_id() == stock._id() 
                   && te._quarter() == 0 ).empty())
    {
        tResults.addFailure(testName + "No record was added to DB");       
        return tResults.getResultsSummary();
    }
    O_Ep f2013 = te.select( te._stock_id() == stock._id() 
                               && te._quarter() == 0 ).front();
    if (f2013._revenue() != "146917000000")
        tResults.addFailure(testName + "Revenue should be: 146917000000, but is: " + f2013._revenue() );
    if (f2013._net_income() != "7155000000")   
        tResults.addFailure(testName + "Net Income should be: 7155000000, but is: " + f2013._net_income() );
    if (f2013._eps() != 1.76)   
        tResults.addFailure(testName + "(diluted) Eps should be: 1.76, but is: " + to_string(f2013._eps()) );
    // test clean up
    te.erase( te._id() == f2013._id());

    //BALANCE extraction test
    O_BalanceSheet b_f2013 = tb.select( tb._stock_id() == stock._id()
                              && tb._year() == year ).front();
    if (b_f2013._current_assets() != "")
        tResults.addFailure(balanceTestName + " CA should be: , but is: " + b_f2013._current_assets() );
    if (b_f2013._total_assets() != "202026000000")
        tResults.addFailure(balanceTestName + " TA should be: 202026000000, but is: " + b_f2013._total_assets() );
    if (b_f2013._current_liabilities() != "")
        tResults.addFailure(balanceTestName + " CL should be: , but is: " + b_f2013._current_liabilities() );
    if (b_f2013._total_liabilities() != "175279000000")
        tResults.addFailure(balanceTestName + " TL should be: 175279000000, but is: " + b_f2013._total_liabilities() );
    if (stock._has_currant_ratio() == true)
        tResults.addFailure(balanceTestName + " NO current ratios, so stock._has_currant() should be false" );
    if (b_f2013._long_term_debt() != "114688000000")
        tResults.addFailure(balanceTestName + " LTD should be: 114688000000, but is: " + b_f2013._long_term_debt() );
    if (b_f2013._book_value() != "26383000000")
        tResults.addFailure(balanceTestName + " BV should be: 26383000000, but is: " + b_f2013._book_value() );
    // test clean up
    tb.erase( tb._id() == b_f2013._id());


    return tResults.getResultsSummary();
}

string
Test::runSingleQarterTest(TestResults& tResults)
{
    LOG_INFO << "\n --- Running runSingleQarterTest() ---\n";
    string testName("Test-Single Quarter 10Q retrieval: ");

    LOG_INFO << "single quarter test under restructuring\n";
    return "";

    T_Stock ts;
    T_Ep te;
    O_Stock stock = ts.select( ts._ticker() == string("CVX")).front();

    string filing = getMockFromDisk("CVX_2014_1stQ.txt");
    if ( filing == "")
    {
        tResults.addFailure(testName + "Could not load mock 10q for testing");
        return tResults.getResultsSummary();
    }
    EdgarData edgar;
    Acn acn( string("0000093410-14-000024"), date(2014,May,2), 1 );
    edgar.addSingleQuarterIncomeStatmentToDB(filing,stock,acn._report_date.year(),acn._quarter);

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
    size_t year = 2013;
    EdgarData edgar;
    edgar.extract10kToDisk( filing, stock, year);
    edgar.createFourthQuarter(stock, year);

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

    size_t year = 2013;
    T_Stock rts;
    T_Ep rte;
    const O_Stock rstock = rts.select( rts._ticker() == ticker).front();
    const O_Ep real2013 = rte.select( rte._stock_id() == rstock._id() 
                               && rte._quarter() == 0
                               && rte._year() == year).front();
      
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

    EdgarData edgar;
    if ( edgar.getSingleYear( stock, year) )
    {
        compareTest( rstock , real2013, testRes);
    }else{
        testRes.addFailure("Could Not get annual data for " + stock._ticker());
    }
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
            stock._country() = it->_country();
            stock._listed() = it->_listed();
            stock._company_type() = it->_company_type();
            stock._name() = it->_name();
            stock.insert();
            LOG_INFO << "Seeded " <<stock._ticker() << " to TEST DB\n";
            cout << "Seeded " <<stock._ticker() << " to TEST DB\n";
        }else{
            LOG_INFO <<it->_ticker() << "Already in TEST DB\n";
            O_Stock stock = ts.select( ts._ticker() == it->_ticker()).front();
            stock._country() = it->_country();
            stock._listed() = it->_listed();
            stock._company_type() = it->_company_type();
            stock.update();
            cout <<it->_ticker() << "Already in TEST DB - UPDATED"<<endl;
        }
    }
}

void
Test::runGetQartersTM(O_Stock& stock){
    LOG_INFO << "\n --- Testing retreval of latest quarters in test mode for " << stock._ticker()
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
    //bool repFail(true);
    TestResults testRes;
    string testName(stock._ticker() + ": ");
    testRes.setTestName(testName);

    EdgarData edgar;
    edgar.updateFinancials(stock);


    // delete added quarters from test DB

    T_Ep te;
    vector<DMMM::O_Ep> eps_copy = stock._eps();
    for(auto it = eps_copy.begin(); it != eps_copy.end();++it)
        if (it->_quarter() < 5)
            te.erase( te._id() ==  it->_id() );

}

bool 
Test::getReportsTest(O_Stock& stock, boost::filesystem::ofstream& outFile)
{
    LOG_INFO << "\n --- Testing retreval of reports for " << stock._ticker()
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
    /*
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
*/
    edgar.getSingleYear(stock,2013);
    auto reports = &edgar._reports;
    bool foreign = false;
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
        if ( (stock._country() != "") &&
             (stock._country() != "USA") )
            foreign = true;
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
        if (foreign)
            outFile << stock._ticker() << " is foreign - " << stock._country();
        outFile << resultSummary;
        outFile.flush();
    }
    LOG_INFO << resultSummary;
    cout << "\n ---  TEST Results for "<<stock._ticker()<<" ---" <<resultSummary << endl;
    return repFail;
}

bool
Test::updateTest(O_Stock& stock)
{
    LOG_INFO << "\n --- Testing retreval of data for " << stock._ticker()
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

    edgar.updateFinancials( stock );
// test covreage (if data missing)
// income
//  balance_

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
