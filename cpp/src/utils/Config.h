#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h>
#include <iostream>
#include <map>
#include <string>
#include <iosfwd>
#include "Utils.hpp"
#include "Logger.h"

class WebDB;

class Config
{
public:
    Config(std::string fileName, int argc, char* argv[]);
    ~Config();

    static Config* instance();

    typedef std::map<std::string, std::string> Params2ValuesMap;

    const Params2ValuesMap& getParams() const { return mParams; }

    void synchWithDB();
    
    void setSandboxing() { mSandbox = true; }

    bool sandboxing() { return mSandbox; }

private:
    //no copying allowed
    Config(const Config& other);
    Config(Config& other);
    Config operator = (const Config& other);

    void readConfFile(const std::string& fileName);
    void setParamsFromCommandLine(int argc, char *argv[]);

    bool setParam(const std::string& line);

private:
    Params2ValuesMap mParams;
    bool           mSandbox;
    static Config* mThis;
};

bool haveParam(const std::string& param);

template <class T>
T
confParam(const std::string& param)
{
    T ret;
    
    Config* conf = Config::instance();
    const Config::Params2ValuesMap& paramsMap = conf->getParams();
    Config::Params2ValuesMap::const_iterator iFind = paramsMap.find(param);
    if (iFind == paramsMap.end()){
    	LOG_ERROR << ("Config param " + param)
                << " doesn't exist";
        exit(-1);
    }
    else
        ret = fromString<T>(iFind->second);

    return ret;
}

void dumpConf(std::ostream& os);


#endif //CONFIG_H
