#include <boost/date_time/posix_time/posix_time.hpp>
#include "Utils.hpp"
#include "CurrentTime.h"

using namespace std;

string 
currentDateTime()
{
    using namespace boost::posix_time;
    ptime now = second_clock::local_time();

    return to_simple_string(now);
}

string 
currentTime()
{
    using namespace boost::posix_time;
    ptime now = second_clock::local_time();

    return to_simple_string(now.time_of_day());
}

 
