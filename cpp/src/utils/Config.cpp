#include <iostream>
#include <fstream>
#include <assert.h>
#include "dmmm_dbface.h"

#include "Config.h"

using namespace std;
using namespace DMMM;

Config* Config::mThis = NULL;
Config::Config(std::string fileName, int argc, char *argv[])
{
    assert(mThis == NULL);
    mThis = this;
    mSandbox = false;
    readConfFile(fileName);
    setParamsFromCommandLine(argc, argv);
}

Config::~Config()
{
}

Config* 
Config::instance()
{
    return mThis;
}

void
Config::setParamsFromCommandLine(int argc, char* argv[])
{
    for (size_t i = 0; i < (size_t)argc; ++i){
        string argvi("argv");
        argvi += toString(i);
        mParams[argvi] = argv[i];
    }
    
//     for (int i = 2; i < argc; ++i)
//         if (setParam(string(argv[i])))
//             cout << "Set parameter from command line: " << argv[i] << endl;
//         else{
//             cout << "Problem with command line parameter " << i <<
//                 " : " << argv[i] << endl;
//             cout << "Format is: " << endl
//                  << "    module.parameter_name = value" << endl; 
//             exit(-1);
//         }
}

bool
Config::setParam(const string& line)
{
    if (line.size() == 0 || line[0] == '#') //# == comment
        return true;

    vector<string> lineParts;        
    splitString(line, ' ', lineParts);
    if (lineParts.size() == 2)
        lineParts.push_back(string());
    if (lineParts.size() != 3 || lineParts[1] != string("="))
        return false;
    else{
        mParams[lineParts[0]] = lineParts[2];
        return true;
    }
}

void
Config::readConfFile(const string& fileName)
{
    ifstream confFile(fileName.c_str());
    
    if (!confFile.is_open()){
        cout << "Could not open configuration file " << fileName << endl;
        exit(1);
    }
    size_t lineNum = 0;
    while (!confFile.eof()){
        string line;
        getline(confFile, line);
        ++lineNum;

        if (!setParam(line)){
            cout << "problem with configuration line " << lineNum;
            cout << ": " << line << endl;
            cout << "Line format is: " << endl
                 << "    module.parameter_name = value" << endl; 
            exit(1);
        }
    }
    confFile.close();
}

void
dumpConf(ostream& os)
{
    Config* conf = Config::instance();
    const Config::Params2ValuesMap& paramsMap = conf->getParams();
    for (auto it = paramsMap.begin(); it != paramsMap.end(); ++it)
        os << "# " << it->first << " = " << it->second << endl; 
    
}

string
synchParam(const string& group, const string& name, 
	   const string& shortName, 
	   const string& defaultVal, const string& valType)
{
    return "nothing to return";	
}

string
synchParamSandbox(const string& group, const string& name, 
		  const string& shortName, 
		  const string& defaultVal, const string& valType)
{
    return "nothing to return";	
}


void
Config::synchWithDB()
{
    string fileName = confParam<string>("db_params_file");
    ifstream confFile(fileName.c_str());
    if (!confFile.is_open()){
        cout << "Could not open configuration file " << fileName << endl;
        exit(1);
    }

    size_t lineNum = 0;
    while (!confFile.eof()){
        string line;
        getline(confFile, line);
        ++lineNum;
        if (line.size() == 0 || line[0] == '#') //# == comment
            continue;
        
        vector<string> splits;
        splitString(line, ';', splits);
        if (splits.size() != 5){
            cerr << "wrong number of parameters on line " << lineNum;
            continue;
        }
        string group = trimSpaces(splits[0]);
        string name = trimSpaces(splits[1]);
        string shortName = trimSpaces(splits[2]);
        string defaultVal = trimSpaces(splits[3]);
        string valType = trimSpaces(toLower(splits[4]));
	if (mSandbox)
	    mParams[shortName] = 
		synchParamSandbox(group, name, shortName, defaultVal, valType);
	else
	    mParams[shortName] = 
		synchParam(group, name, shortName, defaultVal, valType);
    }
    confFile.close();
    
}

bool 
haveParam(const std::string& param)
{
    Config* conf = Config::instance();
    const Config::Params2ValuesMap& paramsMap = conf->getParams();
    auto iFind = paramsMap.find(param);
    return iFind != paramsMap.end();
}
