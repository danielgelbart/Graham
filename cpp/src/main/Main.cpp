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

//#include "T_Fighter.hpp"
//#include "T_Fight.hpp"
//#include <Rcpp.h>
#include <RInside.h>    
//#include <apop.h>

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


void
run_mle(int argc, char* argv[])
{

    // initialize R
    RInside R(argc, argv); 

    // load BradleyTerry library
    // load data to R object
    // Run the MLE
    // Do something with results

    std::string str = 
        "cat('Requireing libraray\n');library('BradleyTerry2'); "
        "cat('Loading data from file\n'); data <- read.table('data',sep=',') ; "
        "cat('Unifiying levels');"
        "uninames <- union(levels(data$fighter1),levels(data$fighter2));"
        "data$fighter1 <-factor(data$fighter1,levels=uninames);"
        "data$fighter2 <-factor(data$fighter2,levels=uninames);"
        "cat('Running BTm()\n');fighterModel <- BTm(cbind(win1, win2), fighter1, fighter2, ~ fighter, id='fighter', data=data) ; "
        "BTabilities(fighterModel)"; // returns a matrix of two colums: fighter; ability
    
    Rcpp::NumericMatrix m = R.parseEval(str);   // eval string, return value then as signed to num. vec              

    for (int i=0; i< m.nrow(); i++) { 
        cout << "Figher " << i << " has skill " << m(i,0) << endl;
    }
    cout << endl;

}

int
mainMain(int argc, char* argv[])
{
    cout << "----------------" << endl;
    cout << "Running Analysis" << endl;

    path binPath(argv[0]);

    cout << "BinPath is " << binPath << endl;
    string command = argc >= 2 ? string(argv[1]) : string();

    if (command == string("--help") || command == string("-h")){
        showHelpMessage(argv[0]);
        exit(-1);
    }
    
    path basePath = binPath.remove_leaf() / "../";
    path confFile = basePath / "bets.conf";
        
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
    

    if (command != string("")){
        showHelpMessage(argv[0]);
        LOG_ERROR << "unknown command: " << command;
    }


    run_mle(argc,argv);

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
