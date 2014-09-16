#ifndef DMMM_COMPARATORS_H
#define DMMM_COMPARATORS_H

#include <string>
#include <sstream>

namespace DMMM {
template<class E>
typename E::ConditionType operator== (E e, const typename E::ComparerType& x){
    typename E::ConditionType c;
    c._cond = e._field + " = '" + UTILS::toString(x) + "'";
    return c;
}

template<class E>
typename E::ConditionType operator< (E e, const typename E::ComparerType& x){
    typename E::ConditionType c;
    c._cond = e._field + " < '" + UTILS::toString(x) + "'";
    return c;
}

template<class E>
typename E::ConditionType operator<= (E e, const typename E::ComparerType& x){
    typename E::ConditionType c;
    c._cond = e._field + " <= '" + UTILS::toString(x) + "'";
    return c;
}

template<class E>
typename E::ConditionType operator> (E e, const typename E::ComparerType& x){
    typename E::ConditionType c;
    c._cond = e._field + " > '" + UTILS::toString(x) + "'";
    return c;
}

template<class E>
typename E::ConditionType operator>= (E e, const typename E::ComparerType& x){
    typename E::ConditionType c;
    c._cond = e._field + " >= '" + UTILS::toString(x) + "'";
    return c;
}

template<class E>
typename E::ConditionType operator!= (E e, const typename E::ComparerType& x){
    typename E::ConditionType c;
    c._cond = e._field + " != '" + UTILS::toString(x) + "'";
    return c;
}

template<class E>
typename E::ConditionType operator%= (E e, const typename E::ComparerType& x){
    typename E::ConditionType c;
    c._cond = e._field + " LIKE '" + UTILS::toString(x) + "'";
    return c;
}

template<class C>
C operator&& (const C& c1, const C& c2){
    C c;
    c._cond = "(" + c1._cond + ") AND (" + c2._cond + ")";
    return c;
}

template<class C>
C operator|| (const C& c1, const C& c2){
    C c;
    c._cond = "(" + c1._cond + ") OR (" + c2._cond + ")";
    return c;
}


} //namespace DMMM

#endif //DMMM_COMPARATORS_H
