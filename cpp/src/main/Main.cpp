#include <sys/types.h>
#include <unistd.h>
#include <boost/filesystem.hpp>   
#include <boost/filesystem/fstream.hpp>
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string.hpp"
#include <iostream>

#include "Identifier.h"
#include "Config.h"
#include "Logger.h"
#include "dmmm_dbface.h"

#include "T_Stock.hpp"

#include "Financials.h"
#include "Test.h"

using namespace DMMM;
using namespace boost::filesystem;
using namespace std;
namespace nsBFS = boost::filesystem;

void 
handleBreak(int sig)
{
    exit(-1);
}

void
showHelpMessage(const string& exPath)
{
    cout << "usage: " << exPath 
         << " <command> <parameter>" << endl << endl
         << "Possible commands and how they're used:" << endl
         << endl;
}


void
dirFor(const path& logDir)
{
    if (!exists(logDir)){
      cout << "Creating log directory " << logDir << endl;
      create_directory(logDir);
    }
}

O_Stock
findStockByTicker( char* cticker )
{
    string ticker(cticker);
    T_Stock ts;
    return ts.select( ts._ticker() == ticker ).front();
}


int
mainMain(int argc, char* argv[])
{
    cout << "----------------" << endl;
    cout << "Running Edgar Data Retrival" << endl;

    path binPath(argv[0]);

    cout << "BinPath is " << binPath << endl;
    string command = argc >= 2 ? string(argv[1]) : string();

    if (command == string("--help") || command == string("-h")){
        showHelpMessage(argv[0]);
        exit(-1);
    }
    
    path basePath = binPath.remove_leaf() / "../";

    if (command == string("test")){
        cout << "\n ---  Running TEST mode ---"<< endl;
        path confFile = basePath / "test_conf.conf";
        cout << "Loading configuration file " << confFile.string() << endl;
        Config* config = new Config(confFile.string().c_str(), argc, argv);

        // set up test logger
        path logFile;
        path logDir;
        logDir = basePath / path(confParam<string>("log_file_name"));
        dirFor(logDir);
        logFile = logDir / string("test_log.txt");       
        cout << "Using test logger" << endl;
        Logger* logger = new Logger(logFile.string().c_str());

        // use TEST DB!!!
        string host = confParam<string>("db.host");
        string user = confParam<string>("db.user");
        string database = confParam<string>("db.database");
        string password = confParam<string>("db.password");
        DMMM::DBFace dbFace(database, host, user, password, logger->logFile());

        Test test;
        if (argc > 2)
        {       
            cout << "\n Callling company test method" << endl;
            string ticker(argv[2]);
            test.runCompanyTest( ticker );
        }
        else
            test.run_all();

        delete(config);
        delete(logger);
        exit(0);
    }

    path confFile = basePath / "conf.conf";
        
    cout << "Loading configuration file " << confFile.string() << endl;
    Config* config = new Config(confFile.string().c_str(), argc, argv);
        
    path logFile;
    path logDir;

    logDir = basePath /
        path(confParam<string>("log_file_name"));

    dirFor(logDir);
    logFile = logDir / string("log.txt");       
    Logger* logger = new Logger(logFile.string().c_str());
         
    LOG_INFO << "Analysing data " << confParam<string>("version");
    LOG_INFO << "Command line arguments: " << argv[0]; 

    string host = confParam<string>("db.host");
    string user = confParam<string>("db.user");
    string database = confParam<string>("db.database");
    string password = confParam<string>("db.password");
    DMMM::DBFace dbFace(database, host, user, password, logger->logFile());
        
// find rigt command to execute and call relavent comand methods
    if (command == string("update_financials")){
        EdgarData eData = EdgarData();
        O_Stock stock = findStockByTicker( argv[2] );
        eData.updateFinancials( stock );
    }
    if (command == string("get_single_year")){
        EdgarData eData = EdgarData();
        O_Stock stock = findStockByTicker( argv[2] );
        
        eData.getSingleYear( stock,2013 );
    }
// should be passed a ticker?
    if (command == string("get_quarters")){
        EdgarData eData = EdgarData();
//        string ticker("IBM");
        O_Stock stock = findStockByTicker( argv[2] );
        eData.getQuarters( stock );
    }

    if (command == string("")){
        showHelpMessage(argv[0]);
        LOG_ERROR << "unknown command: " << command;
    }

    LOG_INFO << "deleting Config";
    delete config;
    
    LOG_INFO << "Last log message: deleting Logger";
    delete logger;

    return 0;
} 

int
main(int argc, char* argv[])
{
    try{
        return mainMain(argc, argv);
    }
    catch(exception& e){
        cout << e.what();
        flush(cout);
        LOG_ERROR << e.what();
    }
    return -1;
}
