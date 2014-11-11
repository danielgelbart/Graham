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

#include "Financials.h"

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
test(Logger* logger)
{
    cout << "Number of errors: " << logger->numErrors() << endl;
    cout << "Number of warnings: " << logger->numWarning() << endl;
    if (logger->numErrors() == 0 && logger->numWarning() == 0)
        cout << "OK" << endl;
}

void
dirFor(const path& logDir)
{
    if (!exists(logDir)){
      cout << "Creating log directory " << logDir << endl;
      create_directory(logDir);
    }
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
    path confFile = basePath / "conf.conf";
        
    cout << "Loading configuration file " << confFile.string() << endl;
    Config* config = new Config(confFile.string().c_str(), argc, argv);
        
    path logFile;
    path logDir;

    logDir = basePath /
        path(confParam<string>("log_file_name"));

    dirFor(logDir);
    logFile = logDir / string(".txt");       
    cout << "Going to create logger" << endl;
     
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
        eData.updateFinancials();
    }
// find rigt command to execute and call relavent comand methods
    if (command == string("xml")){
        EdgarData eData = EdgarData();
        eData.parseStatementsToDB();

    }
// should be passed a ticker?
    if (command == string("update_10k")){
        EdgarData eData = EdgarData();
        eData.updateFinancials();
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
