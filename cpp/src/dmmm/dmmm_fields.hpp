#ifndef DMMM_FIELDS_H
#define DMMM_FIELDS_H

#include <string>

#include <sstream>

#include "dmmm_utils.hpp"

namespace DMMM {

template<class T>
struct Field{
    Field<T>() :_dirty(false) {}
    Field<T>(const T& t) :_dirty(true), _base(t) {}
    typedef T Base;
    bool _dirty;
    T _base;
}; //class Field

typedef Field<int> F_Integer;
typedef Field<double> F_Float;
typedef Field<std::string> F_String;
typedef Field<bool> F_TrueClass;
typedef Field<std::string> F_DateTime;
typedef Field<std::string> F_Date;

// fields for activerecord
typedef Field<double> F_BigDecimal;
typedef Field<int> F_Fixnum;
typedef Field<std::string> F_Time;
typedef Field<bool> F_Object;

} //namespace DMMM

#endif //DMMM_FIELDS_H
