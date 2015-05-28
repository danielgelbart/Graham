#include <iostream>
#include <cmath>
#include "Dates.hpp"

using namespace boost::filesystem;
using namespace boost::gregorian;
using namespace std;

// expects format of: Dec. 23, 2013
date
convertFromDocString(string& docStr)
{
    std::stringstream ss(docStr);
    date_input_facet *df = new date_input_facet("%b. %d, %Y");
    ss.imbue(std::locale(ss.getloc(), df));
    date d;
    ss >> d;  //conversion fails to not-a-date-time
//    std::cout << "'" << d << "'" << std::endl;  //'not-a-date-time' 
    return d;
}

bool
withinAweek(string fd1, string fd2){
    date d1 = convertFyedStringToDate(2015,fd1);
    date d2 = convertFyedStringToDate(2015,fd2);
    date_duration dd = d2 - d1;
    return abs(dd.days()) < 7;
}

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

    // endDate.year == year + 1 
    if (annualendDate.month() < Jun )
    {
        date adj_annualendDate = convertFyedStringToDate( (year + 1), fyenStr);
        return adj_annualendDate;
    }
    if( quarter == 0)
        return annualendDate;

    // if quarter > 0
    date endDate = annualendDate - ( months(3) * (4 - quarter) );
    return endDate;
}

greg_year 
calculateYearFor(string fyenStr, greg_year yyear)
{
    date extracted_date = convertFyedStringToDate( yyear, fyenStr ); 
    date cutoff_date(extracted_date.year(),Jan,30);
    if ( extracted_date < cutoff_date )
        extracted_date.year() = (yyear - 1);
    return extracted_date.year();
}
