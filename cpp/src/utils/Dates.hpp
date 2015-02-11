#ifndef DATES_HPP
#define DATES_HPP

#include <string>
//#include <sstream>
#include <boost/filesystem.hpp>
//#include "Logger.h"
#include <boost/date_time/gregorian/gregorian.hpp>

using namespace boost::filesystem;
using namespace boost::gregorian;
//using namespace DMMM;
using namespace std;

date convertFromDocString(string& inp);
date convertFyedStringToDate(greg_year year,string fyenStr);
date calculateEndDate(string fyenStr, greg_year year, size_t quarter);
greg_year calculateYearFor(string fyenStr, greg_year year);

#endif //UTILS_HPP
