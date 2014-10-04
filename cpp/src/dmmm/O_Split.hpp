#ifndef O_SPLIT
#define O_SPLIT

#include <map>
#include <string>
#include "dmmm_utils.hpp"
#include "dmmm_dbface.h"
#include "dmmm_identifiers.hpp"
#include "dmmm_fields.hpp"
#include "dmmm_comparators.hpp"



namespace DMMM {

class O_Split{
public:

    O_Split() {}
    O_Split(const I_Stock& parent_id) 
        : _f_stock_id(parent_id)
        {}

O_Split(const I_Split& id) 
        : _f_id(id)
        {}


    const Field<I_Split>::Base& _id() const { 
        return _f_id._base; 
    }
    Field<I_Split>::Base& _id() { 
        _f_id._dirty = true; 
        return _f_id._base; 
    }
    const F_Fixnum::Base& _stock_id() const { 
        return _f_stock_id._base; 
    }
    F_Fixnum::Base& _stock_id() { 
        _f_stock_id._dirty = true; 
        return _f_stock_id._base; 
    }
    const F_Fixnum::Base& _old() const { 
        return _f_old._base; 
    }
    F_Fixnum::Base& _old() { 
        _f_old._dirty = true; 
        return _f_old._base; 
    }
    const F_Fixnum::Base& _new() const { 
        return _f_new._base; 
    }
    F_Fixnum::Base& _new() { 
        _f_new._dirty = true; 
        return _f_new._base; 
    }
    const F_BigDecimal::Base& _split_mul_factor() const { 
        return _f_split_mul_factor._base; 
    }
    F_BigDecimal::Base& _split_mul_factor() { 
        _f_split_mul_factor._dirty = true; 
        return _f_split_mul_factor._base; 
    }
    const F_Date::Base& _date() const { 
        return _f_date._base; 
    }
    F_Date::Base& _date() { 
        _f_date._dirty = true; 
        return _f_date._base; 
    }
    const F_Object::Base& _stock_adjusted_for_split() const { 
        return _f_stock_adjusted_for_split._base; 
    }
    F_Object::Base& _stock_adjusted_for_split() { 
        _f_stock_adjusted_for_split._dirty = true; 
        return _f_stock_adjusted_for_split._base; 
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

    bool update(){
        std::map<std::string, std::string> field2Val;
        if (_f_id._dirty)
            field2Val["id"] = 
                toSQLString(_f_id._base);
        if (_f_stock_id._dirty)
            field2Val["stock_id"] = 
                toSQLString(_f_stock_id._base);
        if (_f_old._dirty)
            field2Val["old"] = 
                toSQLString(_f_old._base);
        if (_f_new._dirty)
            field2Val["new"] = 
                toSQLString(_f_new._base);
        if (_f_split_mul_factor._dirty)
            field2Val["split_mul_factor"] = 
                toSQLString(_f_split_mul_factor._base);
        if (_f_date._dirty)
            field2Val["date"] = 
                toSQLString(_f_date._base);
        if (_f_stock_adjusted_for_split._dirty)
            field2Val["stock_adjusted_for_split"] = 
                toSQLString(_f_stock_adjusted_for_split._base);
        if (_f_created_at._dirty)
            field2Val["created_at"] = 
                toSQLString(_f_created_at._base);

        field2Val["updated_at"] = "NOW()";

        std::string where =
            "id=" + toSQLString(_f_id._base);
        if (DBFace::instance()->update("splits", 
                                       field2Val, where))
        {
            _f_id._dirty = false;
            _f_stock_id._dirty = false;
            _f_old._dirty = false;
            _f_new._dirty = false;
            _f_split_mul_factor._dirty = false;
            _f_date._dirty = false;
            _f_stock_adjusted_for_split._dirty = false;
            _f_created_at._dirty = false;
            _f_updated_at._dirty = false;
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
        if (_f_stock_id._dirty)
            field2Val["stock_id"] = 
                toSQLString(_f_stock_id._base);
        if (_f_old._dirty)
            field2Val["old"] = 
                toSQLString(_f_old._base);
        if (_f_new._dirty)
            field2Val["new"] = 
                toSQLString(_f_new._base);
        if (_f_split_mul_factor._dirty)
            field2Val["split_mul_factor"] = 
                toSQLString(_f_split_mul_factor._base);
        if (_f_date._dirty)
            field2Val["date"] = 
                toSQLString(_f_date._base);
        if (_f_stock_adjusted_for_split._dirty)
            field2Val["stock_adjusted_for_split"] = 
                toSQLString(_f_stock_adjusted_for_split._base);

        field2Val["created_at"] = "NOW()";

        if (_f_updated_at._dirty)
            field2Val["updated_at"] = 
                toSQLString(_f_updated_at._base);

        
        if (DBFace::instance()->
                insert("splits", field2Val,
                       _f_id._base.serialization()))
        {
            _f_id._dirty = false;
            _f_stock_id._dirty = false;
            _f_old._dirty = false;
            _f_new._dirty = false;
            _f_split_mul_factor._dirty = false;
            _f_date._dirty = false;
            _f_stock_adjusted_for_split._dirty = false;
            _f_created_at._dirty = false;
            _f_updated_at._dirty = false;
            return true;
        }
        else
            return false;
                                   
    }
private:
    Field<I_Split> _f_id;
    F_Fixnum _f_stock_id;
    F_Fixnum _f_old;
    F_Fixnum _f_new;
    F_BigDecimal _f_split_mul_factor;
    F_Date _f_date;
    F_Object _f_stock_adjusted_for_split;
    F_Time _f_created_at;
    F_Time _f_updated_at;

    friend class T_Split;
};

} //namespace DMMM
#endif //O_SPLIT
