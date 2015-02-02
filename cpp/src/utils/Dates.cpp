
#include "Dates.hpp"

using namespace boost::filesystem;
using namespace boost::gregorian;
//using namespace DMMM;
using namespace std;

date
convertFyedStringToDate(greg_year year,string fyenStr)
{
    if ( fyenStr.length() != 5 )
        fyenStr = "12-31";
    string daysStr = fyenStr.substr(3);
    string monthsStr = fyenStr.substr(0,2);
    date annualendDate(year,
                       greg_month(stoi(monthsStr)), 
                       greg_day(stoi(daysStr)));
    return annualendDate;
}

date 
calculateEndDate(string fyenStr, greg_year year, size_t quarter)
{
    date annualendDate = convertFyedStringToDate(year,fyenStr);
    date endDate = annualendDate - (months(3) * quarter);
//    LOG_INFO << "\n Calculated end date for ";
    return endDate;
}
