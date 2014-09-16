#ifndef DMMM_UTILS_HPP
#define DMMM_UTILS_HPP

#include <vector>
#include <map>
#include <sstream>
#include "dmmm_id.hpp"

namespace DMMM {

typedef std::vector<std::string> FieldVector;
struct FieldVectorComp{
    bool operator() (const FieldVector& f1, const FieldVector& f2) const
    {
        if (f1.size() < f2.size())
            return true;
        if (f1.size() > f2.size())
            return false;
        for (size_t i = 0; i < f1.size(); ++i){
            if (f1[i] < f2[i])
                return true;
            if (f1[i] > f2[i])
                return false;
        }
        return false;
    }
};

typedef std::map<FieldVector, 
                 std::vector<std::vector<std::string> >,
                 FieldVectorComp> FieldsToRows;

namespace UTILS {

template<class T>
std::string
toString(const T& t)
{
    std::ostringstream oss;
    oss << t;
    
    return oss.str();
}

template<class T>
T
fromString(const std::string& s)
{
    std::istringstream iss;
    iss.str(s);
    
    T ret;
    iss >> ret;
    
    return ret;
}

template<>
std::string
fromString<std::string>(const std::string& s);

} //namespace UTILS
} //namespace DMMM

#endif //DMMM_UTILS_HPP
