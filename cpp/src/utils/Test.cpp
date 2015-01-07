#include <assert.h>
#include "Logger.h"
#include "Config.h"
#include "Utils.hpp"
#include "info.h"
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
getMock10kFromDisk()
{       
    string fileName("IBM_2013");
    path binPath = confParam<string>("argv0");
    path filePath = binPath.remove_filename() / 
        "../../financials/IBM" / fileName;

    cout << "going to load file at " << filePath.string();
    string filing =  loadFileToString(filePath.string());
    return filing;
}

string
Test::runSingelYearTest(TestResults& testResults)
{
    LOG_INFO << "\n --- Running runSingelYearTest() ---\n";
    string testName("Test-Single year 10K retrieval: ");
    
    // DB setup
    T_Stock ts;
    ts.erase();
    T_Ep te;
    te.erase();

    O_Stock stock;
    string ticker("IBM");
    stock._ticker() = ticker;
    stock.insert();
    stock = ts.select( ts._ticker() == ticker).front();

    TestResults tResults;
    // extract to DB
    string filing = getMock10kFromDisk();
    if ( filing == "")
    {
        tResults.addFailure(testName + "Could not load mock 10k for testing");          return tResults.getResultsSummary();
    }

    Info info( stock._ticker(), 2013, StatementType::K10);
    EdgarData edgar;
    edgar.extract10kToDisk( filing, stock, info);

    //Test results writen to DB
    if (te.select( te._year() == 2013).empty())
    {
        tResults.addFailure(testName + "No record was added to DB");       
        return tResults.getResultsSummary();
    }

    O_Ep ibm2013 = te.select().front();
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
    
    return tResults.getResultsSummary();
}

void 
Test::run_all()
{
    bool rok(false);
    rok = db_setup();
    if(!rok)
    {
        LOG_ERROR << "Something wrong with TEST. EXITING";
        cout << "An error uccored. exiting";
        exit(-1);
    }

    TestResults testRes;
    // test single year
    runSingelYearTest(testRes);
    
    string resultSummary = testRes.getResultsSummary();
    cout << "\n ---  TEST Results  ---" <<resultSummary << endl;
    
// test quarters

    // test fourth quarter

    // test ttm eps



}
