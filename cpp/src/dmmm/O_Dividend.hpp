#ifndef O_DIVIDEND
#define O_DIVIDEND

#include <map>
#include <string>
#include "dmmm_utils.hpp"
#include "dmmm_dbface.h"
#include "dmmm_identifiers.hpp"
#include "dmmm_fields.hpp"
#include "dmmm_comparators.hpp"



namespace DMMM {

class O_Dividend{
public:

    O_Dividend() {}
    O_Dividend(const I_Stock& parent_id) 
    : _f_stock_id(parent_id)
        {}

O_Dividend(const I_Dividend& id) 
        : _f_id(id)
        {}


    const Field<I_Dividend>::Base& _id() const { 
        return _f_id._base; 
    }
    Field<I_Dividend>::Base& _id() { 
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
    const F_Date::Base& _date() const { 
        return _f_date._base; 
    }
    F_Date::Base& _date() { 
        _f_date._dirty = true; 
        return _f_date._base; 
    }
    const F_BigDecimal::Base& _amount() const { 
        return _f_amount._base; 
    }
    F_BigDecimal::Base& _amount() { 
        _f_amount._dirty = true; 
        return _f_amount._base; 
    }
    const F_String::Base& _source() const { 
        return _f_source._base; 
    }
    F_String::Base& _source() { 
        _f_source._dirty = true; 
        return _f_source._base; 
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
        if (_f_date._dirty)
            field2Val["date"] = 
                toSQLString(_f_date._base);
        if (_f_amount._dirty)
            field2Val["amount"] = 
                toSQLString(_f_amount._base);
        if (_f_source._dirty)
            field2Val["source"] = 
                toSQLString(_f_source._base);
        if (_f_created_at._dirty)
            field2Val["created_at"] = 
                toSQLString(_f_created_at._base);

        field2Val["updated_at"] = "NOW()";

        std::string where =
            "id=" + toSQLString(_f_id._base);
        if (DBFace::instance()->update("dividends", 
                                       field2Val, where))
        {
            _f_id._dirty = false;
            _f_stock_id._dirty = false;
            _f_date._dirty = false;
            _f_amount._dirty = false;
            _f_source._dirty = false;
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
        if (_f_date._dirty)
            field2Val["date"] = 
                toSQLString(_f_date._base);
        if (_f_amount._dirty)
            field2Val["amount"] = 
                toSQLString(_f_amount._base);
        if (_f_source._dirty)
            field2Val["source"] = 
                toSQLString(_f_source._base);

        field2Val["created_at"] = "NOW()";

        if (_f_updated_at._dirty)
            field2Val["updated_at"] = 
                toSQLString(_f_updated_at._base);

        
        if (DBFace::instance()->
                insert("dividends", field2Val,
                       _f_id._base.serialization()))
        {
            _f_id._dirty = false;
            _f_stock_id._dirty = false;
            _f_date._dirty = false;
            _f_amount._dirty = false;
            _f_source._dirty = false;
            _f_created_at._dirty = false;
            _f_updated_at._dirty = false;
            return true;
        }
        else
            return false;
                                   
    }
private:
    Field<I_Dividend> _f_id;
    F_Fixnum _f_stock_id;
    F_Date _f_date;
    F_BigDecimal _f_amount;
    F_String _f_source;
    F_Time _f_created_at;
    F_Time _f_updated_at;

    friend class T_Dividend;
};

} //namespace DMMM
#endif //O_DIVIDEND
