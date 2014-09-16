#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include <string>
#include <fstream>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "Utils.hpp"


using namespace boost::filesystem;
using namespace boost::algorithm;

string
pathFrom(const string& full, const string& from)
{
    path fullPath(full);
    path retPath;

    bool foundFrom = false;
    for (auto it = fullPath.begin(); it != fullPath.end(); ++it){
        if (*it == path(from))
            foundFrom = true;
        if (foundFrom)
            retPath /= *it;
    }
    return toString(retPath);
}

path
head(const path& p)
{
    return *p.begin();
}
 
path
decapitate(const path& p)
{
    path ret;
    for (auto it = p.begin(); it != p.end(); ++it)
        if (it != p.begin())
            ret /= *it;
    return ret;
}

string
toLower(const string& s)
{
    return to_lower_copy(s);
}

string
loadFileToString(const string& fileName)
{
    std::ifstream f(fileName.c_str(), ios::binary);

    f.seekg(0, ios::end);
    std::ifstream::pos_type filesize = f.tellg();
    f.seekg(0, ios::beg);
    
    string ret(filesize, 0);
    f.read(&ret[0], filesize);

    return ret;
}

string
trimSpaces(const string& s)
{
    return trim_copy(s);
}

string 
substitute(const string& orig, const string& cut, const string& paste)
{
    return replace_all_copy(orig,cut,paste);;
}

    
size_t
getMemUsedMB()
{ 
    std::ifstream procFile("/proc/self/status");
    
    if (!procFile.is_open())
        return 0;

    while (!procFile.eof()){
        string line;
        getline(procFile, line);

        if (line.length() > 5 && line.substr(0, 5) == string("VmRSS"))
            return fromString<size_t>(line.substr(6, 10)) / 1024;
    }   
    return 0;
}
