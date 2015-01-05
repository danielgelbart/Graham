#ifndef TEST_H
#define TEST_H

#include <boost/filesystem.hpp>   
#include <map>

#include "types.h"
#include "Tokenizer.h"
#include "info.h"
#include "O_Stock.hpp"

using namespace std;
using namespace DMMM;
using namespace boost::filesystem;

class Test {
public:
    Test(path p):_mockPath(p){}
    
    void run();
    string runSingelYearTest();

private: //members
    path _mockPath;

}; //end class test
#endif
