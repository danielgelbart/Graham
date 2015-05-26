#ifndef INFO_H
#define INFO_H

#include <vector>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "Dates.hpp"

using namespace boost::gregorian;

enum class StatementType {K10,Q10};

struct Info{
    Info() {}
    Info(string& tik, size_t yy, StatementType tt, size_t qq=0): 
        ticker( tik ), year( yy ), type( tt ), quarter( qq ) {}

// members
    string ticker;
    size_t year;
    StatementType type;
    size_t quarter; // if 10-k, then quaeter==0
};

struct Acn {

    Acn() : _year(2014) {}
    Acn( string acn, date repDate, size_t quarter = 0 ) :
        _acn(acn), _report_date(repDate), _year(1400), _quarter(quarter) {}

    bool
    setAcnYearAndQuarter(date report_date, string fyedStr )
    {
        greg_year report_year = report_date.year();
        // Note - endates are the calculated dates that each fiscal quarter ENDS at
        // endDates vecotr is orders from most recent == first (or 0 position).
        // Also, by construction, endDate.size() <= 12
        vector<date> endDates;
        date future_end_date = convertFyedStringToDate((report_year+1), fyedStr);
        endDates.push_back(future_end_date);

        date temp_date = future_end_date - months(3);
        while( temp_date.year() >= (report_year - 1) )
        {
            endDates.push_back(temp_date);
            temp_date = (temp_date - months(3));
        }

        // find wich date matches the report
        date_period monthsPriorReprot( (report_date - months(3)), report_date );
        LOG_INFO << "Constructed peariod for report"<<monthsPriorReprot.begin()
                    << " to " << monthsPriorReprot.end();
        for(size_t i=0; i<endDates.size(); ++i)
        {
            LOG_INFO << "Iterating over endDate["<<i<<"], it is for date "<< endDates[i];
            if ( monthsPriorReprot.contains( endDates[i]) )
            {
                LOG_INFO<< "FOUND: acn dated "<< report_date << " is for term ended " << endDates[i];
                // 4th quarter is marked as '0' (or annual)
                _quarter = (12 - i) % 4;

                // complete _quareter to end of year
                // then, use year of fiscal year end
                _year = endDates[0].year() ;
                if( i > 3)
                    _year = endDates[4].year();
                if( i > 7)
                    _year = endDates[8].year();

                LOG_INFO << " SET YEAR TO" << _year << " and QUARTER "<< _quarter;
                return true;
            }
        }
        return false;
    }

    // this method depricated
    void set_quarter_from_date(date& endate)
    {
        _year = (endate.year() + 1);

        date_period fourthQ( endate, (endate + months(3)) );
        if ( fourthQ.contains( _report_date) )
        {
            _quarter = 4;
            _year = (endate - days(2)).year();
        }

        date_period firstQ( endate + months(3), (endate + months(6)) );
        if ( firstQ.contains( _report_date) )
            _quarter = 1;

        date_period secondQ( endate + months(6), (endate + months(9)) );
        if ( secondQ.contains( _report_date) )
            _quarter = 2;

        date_period thirdQ( endate + months(9), (endate + months(12)) );
        if ( thirdQ.contains( _report_date) )
            _quarter = 3;
    }
    // members
    string _acn;
    date _report_date;
    greg_year _year;
    int _quarter;
};


#endif
