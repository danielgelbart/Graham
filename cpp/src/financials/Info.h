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

    Acn() {}
    Acn( string acn, date repDate, size_t quarter ) :
        _acn(acn), _report_date(repDate), _quarter(quarter) {}

    void set_quarter_from_date()
        {
            greg_year year = _report_date.year();
            date_period secondQ( date(year,Apr,1), date(year,Jun,30) );
            if ( secondQ.contains( _report_date) )
                _quarter = 1;
            date_period thirdQ( date(year,Jul,1), date(year,Sep,30) );
            if ( thirdQ.contains( _report_date) )
                _quarter = 2;
            date_period fourthQ( date(year,Oct,1), date(year,Dec,31) );
            if ( fourthQ.contains( _report_date) )
                _quarter = 3;
        }
// members
    string _acn;
    date _report_date;
    int _quarter;
};


#endif
