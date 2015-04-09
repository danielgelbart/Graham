#ifndef INFO_H
#define INFO_H

#include <boost/date_time/gregorian/gregorian.hpp>

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
    Acn( string acn, date repDate, size_t quarter ) :
        _acn(acn), _report_date(repDate), _year(repDate.year()-1), _quarter(quarter) {}

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
