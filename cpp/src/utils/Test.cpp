#include <assert.h>
#include "Logger.h"
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
getMock10kFromDisk(path& p)
{       
    string fileName("IBM_2013");
    path filePath = p / filePath;

    cout << "going to load file";
    string filing =  loadFileToString(filePath.string());
    cout << "got filing";
    return filing;
}

string
Test::runSingelYearTest()
{
    LOG_INFO << "\n --- Running runSingelYearTest() ---\n";
    string retStr("");
    
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

    // extract to DB
    string filing = getMock10kFromDisk(_mockPath);
    cout << "\n loaded file starting with: "<< filing.substr(0,300)<<endl;
    Info info( stock._ticker(), 2013, StatementType::K10);
    EdgarData edgar;
    edgar.extract10kToDisk( filing, stock, info);
    
    //Test results writen to DB
    return retStr;
}

void 
Test::run()
{
    bool rok(false);
    rok = db_setup();
    if(!rok)
    {
        LOG_ERROR << "Something wrong with TEST. EXITING";
        cout << "An error uccored. exiting";
        exit(-1);
    }

    // test single year
    runSingelYearTest();

    
// test quarters

    // test fourth quarter

    // test ttm eps



}
