#include "stockhelper.h"
#include <stdlib.h>
#include "T_Ep.hpp"

using namespace std;
using namespace DMMM;

struct qep_comp {
    bool operator() (const O_Ep& i, const O_Ep& j) const
        {
        if ( i._year() == j._year() )
            return ( i._quarter() > j._quarter() );
        else
            return (i._year() > j._year() );
        }
} qep_comp;

O_Ep StockHelper::get_latest_quarter(){
    T_Ep t;
    // get all quarter reports, and SORT - NEWEST FIRST
    vector<O_Ep> qrts = _stock._eps( (t._quarter() > 0) &&
                                    (t._quarter() < 5));

    sort( qrts.begin(), qrts.end(), qep_comp );

    return qrts[0];
}
