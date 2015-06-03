#ifndef O_SEARCH
#define O_SEARCH

#include <map>
#include <string>
#include "dmmm_utils.hpp"
#include "dmmm_dbface.h"
#include "dmmm_identifiers.hpp"
#include "dmmm_fields.hpp"
#include "dmmm_comparators.hpp"


namespace DMMM {

class O_Search{
public:

    O_Search() {}

O_Search(const I_Search& id) 
        : _f_id(id)
        {}



    const Field<I_Search>::Base& _id() const { 
        return _f_id._base; 
    }
    Field<I_Search>::Base& _id() { 
        _f_id._dirty = true; 
        return _f_id._base; 
    }
    const F_Object::Base& _no_losses() const { 
        return _f_no_losses._base; 
    }
    F_Object::Base& _no_losses() { 
        _f_no_losses._dirty = true; 
        return _f_no_losses._base; 
    }
    const F_Object::Base& _divs() const { 
        return _f_divs._base; 
    }
    F_Object::Base& _divs() { 
        _f_divs._dirty = true; 
        return _f_divs._base; 
    }
    const F_Float::Base& _dilution() const { 
        return _f_dilution._base; 
    }
    F_Float::Base& _dilution() { 
        _f_dilution._dirty = true; 
        return _f_dilution._base; 
    }
    const F_Time::Base& _created_at() const { 
        return _f_created_at._base; 
    }
    F_Time::Base& _created_at() { 
        _f_created_at._dirty = true; 
        return _f_created_at._base; 
    }
    const F_Time::Base& _updated_at() const { 
        return _f_updated_at._base; 
    }
    F_Time::Base& _updated_at() { 
        _f_updated_at._dirty = true; 
        return _f_updated_at._base; 
    }
    const F_Object::Base& _good_balance() const { 
        return _f_good_balance._base; 
    }
    F_Object::Base& _good_balance() { 
        _f_good_balance._dirty = true; 
        return _f_good_balance._base; 
    }
    const F_Fixnum::Base& _book() const { 
        return _f_book._base; 
    }
    F_Fixnum::Base& _book() { 
        _f_book._dirty = true; 
        return _f_book._base; 
    }
    const F_Object::Base& _current_div() const { 
        return _f_current_div._base; 
    }
    F_Object::Base& _current_div() { 
        _f_current_div._dirty = true; 
        return _f_current_div._base; 
    }
    const F_Object::Base& _earning_growth() const { 
        return _f_earning_growth._base; 
    }
    F_Object::Base& _earning_growth() { 
        _f_earning_growth._dirty = true; 
        return _f_earning_growth._base; 
    }
    const F_Object::Base& _defensive_price() const { 
        return _f_defensive_price._base; 
    }
    F_Object::Base& _defensive_price() { 
        _f_defensive_price._dirty = true; 
        return _f_defensive_price._base; 
    }
    const F_Object::Base& _big_enough() const { 
        return _f_big_enough._base; 
    }
    F_Object::Base& _big_enough() { 
        _f_big_enough._dirty = true; 
        return _f_big_enough._base; 
    }
    const F_String::Base& _market_cap() const { 
        return _f_market_cap._base; 
    }
    F_String::Base& _market_cap() { 
        _f_market_cap._dirty = true; 
        return _f_market_cap._base; 
    }
    const F_String::Base& _sort_by() const { 
        return _f_sort_by._base; 
    }
    F_String::Base& _sort_by() { 
        _f_sort_by._dirty = true; 
        return _f_sort_by._base; 
    }

    bool update(){
        std::map<std::string, std::string> field2Val;
        if (_f_id._dirty)
            field2Val["id"] = 
                toSQLString(_f_id._base);
        if (_f_no_losses._dirty)
            field2Val["no_losses"] = 
                toSQLString(_f_no_losses._base);
        if (_f_divs._dirty)
            field2Val["divs"] = 
                toSQLString(_f_divs._base);
        if (_f_dilution._dirty)
            field2Val["dilution"] = 
                toSQLString(_f_dilution._base);
        if (_f_created_at._dirty)
            field2Val["created_at"] = 
                toSQLString(_f_created_at._base);

        field2Val["updated_at"] = "NOW()";

        if (_f_good_balance._dirty)
            field2Val["good_balance"] = 
                toSQLString(_f_good_balance._base);
        if (_f_book._dirty)
            field2Val["book"] = 
                toSQLString(_f_book._base);
        if (_f_current_div._dirty)
            field2Val["current_div"] = 
                toSQLString(_f_current_div._base);
        if (_f_earning_growth._dirty)
            field2Val["earning_growth"] = 
                toSQLString(_f_earning_growth._base);
        if (_f_defensive_price._dirty)
            field2Val["defensive_price"] = 
                toSQLString(_f_defensive_price._base);
        if (_f_big_enough._dirty)
            field2Val["big_enough"] = 
                toSQLString(_f_big_enough._base);
        if (_f_market_cap._dirty)
            field2Val["market_cap"] = 
                toSQLString(_f_market_cap._base);
        if (_f_sort_by._dirty)
            field2Val["sort_by"] = 
                toSQLString(_f_sort_by._base);
        std::string where =
            "id=" + toSQLString(_f_id._base);
        if (DBFace::instance()->update("searches", 
                                       field2Val, where))
        {
            _f_id._dirty = false;
            _f_no_losses._dirty = false;
            _f_divs._dirty = false;
            _f_dilution._dirty = false;
            _f_created_at._dirty = false;
            _f_updated_at._dirty = false;
            _f_good_balance._dirty = false;
            _f_book._dirty = false;
            _f_current_div._dirty = false;
            _f_earning_growth._dirty = false;
            _f_defensive_price._dirty = false;
            _f_big_enough._dirty = false;
            _f_market_cap._dirty = false;
            _f_sort_by._dirty = false;
            return true;
        }
        else
            return false;
    }

    bool insert(){
        std::map<std::string, std::string> field2Val;
        if (_f_id._dirty)
            field2Val["id"] = 
                toSQLString(_f_id._base);
        if (_f_no_losses._dirty)
            field2Val["no_losses"] = 
                toSQLString(_f_no_losses._base);
        if (_f_divs._dirty)
            field2Val["divs"] = 
                toSQLString(_f_divs._base);
        if (_f_dilution._dirty)
            field2Val["dilution"] = 
                toSQLString(_f_dilution._base);

        field2Val["created_at"] = "NOW()";

        if (_f_updated_at._dirty)
            field2Val["updated_at"] = 
                toSQLString(_f_updated_at._base);
        if (_f_good_balance._dirty)
            field2Val["good_balance"] = 
                toSQLString(_f_good_balance._base);
        if (_f_book._dirty)
            field2Val["book"] = 
                toSQLString(_f_book._base);
        if (_f_current_div._dirty)
            field2Val["current_div"] = 
                toSQLString(_f_current_div._base);
        if (_f_earning_growth._dirty)
            field2Val["earning_growth"] = 
                toSQLString(_f_earning_growth._base);
        if (_f_defensive_price._dirty)
            field2Val["defensive_price"] = 
                toSQLString(_f_defensive_price._base);
        if (_f_big_enough._dirty)
            field2Val["big_enough"] = 
                toSQLString(_f_big_enough._base);
        if (_f_market_cap._dirty)
            field2Val["market_cap"] = 
                toSQLString(_f_market_cap._base);
        if (_f_sort_by._dirty)
            field2Val["sort_by"] = 
                toSQLString(_f_sort_by._base);

        
        if (DBFace::instance()->
                insert("searches", field2Val,
                       _f_id._base.serialization()))
        {
            _f_id._dirty = false;
            _f_no_losses._dirty = false;
            _f_divs._dirty = false;
            _f_dilution._dirty = false;
            _f_created_at._dirty = false;
            _f_updated_at._dirty = false;
            _f_good_balance._dirty = false;
            _f_book._dirty = false;
            _f_current_div._dirty = false;
            _f_earning_growth._dirty = false;
            _f_defensive_price._dirty = false;
            _f_big_enough._dirty = false;
            _f_market_cap._dirty = false;
            _f_sort_by._dirty = false;
            return true;
        }
        else
            return false;
                                   
    }
private:
    Field<I_Search> _f_id;
    F_Object _f_no_losses;
    F_Object _f_divs;
    F_Float _f_dilution;
    F_Time _f_created_at;
    F_Time _f_updated_at;
    F_Object _f_good_balance;
    F_Fixnum _f_book;
    F_Object _f_current_div;
    F_Object _f_earning_growth;
    F_Object _f_defensive_price;
    F_Object _f_big_enough;
    F_String _f_market_cap;
    F_String _f_sort_by;

    friend class T_Search;
};

} //namespace DMMM
#endif //O_SEARCH
