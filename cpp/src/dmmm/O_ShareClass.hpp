#ifndef O_SHARECLASS
#define O_SHARECLASS

#include <map>
#include <string>
#include "dmmm_utils.hpp"
#include "dmmm_dbface.h"
#include "dmmm_identifiers.hpp"
#include "dmmm_fields.hpp"
#include "dmmm_comparators.hpp"


namespace DMMM {

class O_ShareClass{
public:

    O_ShareClass() {}
    O_ShareClass(const I_Stock& parent_id) 
        : _f_stock_id(parent_id)
        {}

O_ShareClass(const I_ShareClass& id) 
        : _f_id(id)
        {}



    const Field<I_ShareClass>::Base& _id() const { 
        return _f_id._base; 
    }
    Field<I_ShareClass>::Base& _id() { 
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
    const F_String::Base& _ticker() const { 
        return _f_ticker._base; 
    }
    F_String::Base& _ticker() { 
        _f_ticker._dirty = true; 
        return _f_ticker._base; 
    }
    const F_String::Base& _sclass() const { 
        return _f_sclass._base; 
    }
    F_String::Base& _sclass() { 
        _f_sclass._dirty = true; 
        return _f_sclass._base; 
    }
    const F_Fixnum::Base& _votes() const { 
        return _f_votes._base; 
    }
    F_Fixnum::Base& _votes() { 
        _f_votes._dirty = true; 
        return _f_votes._base; 
    }
    const F_String::Base& _nshares() const { 
        return _f_nshares._base; 
    }
    F_String::Base& _nshares() { 
        _f_nshares._dirty = true; 
        return _f_nshares._base; 
    }
    const F_Date::Base& _float_date() const { 
        return _f_float_date._base; 
    }
    F_Date::Base& _float_date() { 
        _f_float_date._dirty = true; 
        return _f_float_date._base; 
    }
    const F_BigDecimal::Base& _mul_factor() const { 
        return _f_mul_factor._base; 
    }
    F_BigDecimal::Base& _mul_factor() { 
        _f_mul_factor._dirty = true; 
        return _f_mul_factor._base; 
    }
    const F_String::Base& _note() const { 
        return _f_note._base; 
    }
    F_String::Base& _note() { 
        _f_note._dirty = true; 
        return _f_note._base; 
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
    const F_BigDecimal::Base& _latest_price() const { 
        return _f_latest_price._base; 
    }
    F_BigDecimal::Base& _latest_price() { 
        _f_latest_price._dirty = true; 
        return _f_latest_price._base; 
    }
    const F_Object::Base& _primary_class() const { 
        return _f_primary_class._base; 
    }
    F_Object::Base& _primary_class() { 
        _f_primary_class._dirty = true; 
        return _f_primary_class._base; 
    }

    bool update(){
        std::map<std::string, std::string> field2Val;
        if (_f_id._dirty)
            field2Val["id"] = 
                toSQLString(_f_id._base);
        if (_f_stock_id._dirty)
            field2Val["stock_id"] = 
                toSQLString(_f_stock_id._base);
        if (_f_ticker._dirty)
            field2Val["ticker"] = 
                toSQLString(_f_ticker._base);
        if (_f_sclass._dirty)
            field2Val["sclass"] = 
                toSQLString(_f_sclass._base);
        if (_f_votes._dirty)
            field2Val["votes"] = 
                toSQLString(_f_votes._base);
        if (_f_nshares._dirty)
            field2Val["nshares"] = 
                toSQLString(_f_nshares._base);
        if (_f_float_date._dirty)
            field2Val["float_date"] = 
                toSQLString(_f_float_date._base);
        if (_f_mul_factor._dirty)
            field2Val["mul_factor"] = 
                toSQLString(_f_mul_factor._base);
        if (_f_note._dirty)
            field2Val["note"] = 
                toSQLString(_f_note._base);
        if (_f_created_at._dirty)
            field2Val["created_at"] = 
                toSQLString(_f_created_at._base);

        field2Val["updated_at"] = "NOW()";

        if (_f_latest_price._dirty)
            field2Val["latest_price"] = 
                toSQLString(_f_latest_price._base);
        if (_f_primary_class._dirty)
            field2Val["primary_class"] = 
                toSQLString(_f_primary_class._base);
        std::string where =
            "id=" + toSQLString(_f_id._base);
        if (DBFace::instance()->update("share_classes", 
                                       field2Val, where))
        {
            _f_id._dirty = false;
            _f_stock_id._dirty = false;
            _f_ticker._dirty = false;
            _f_sclass._dirty = false;
            _f_votes._dirty = false;
            _f_nshares._dirty = false;
            _f_float_date._dirty = false;
            _f_mul_factor._dirty = false;
            _f_note._dirty = false;
            _f_created_at._dirty = false;
            _f_updated_at._dirty = false;
            _f_latest_price._dirty = false;
            _f_primary_class._dirty = false;
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
        if (_f_ticker._dirty)
            field2Val["ticker"] = 
                toSQLString(_f_ticker._base);
        if (_f_sclass._dirty)
            field2Val["sclass"] = 
                toSQLString(_f_sclass._base);
        if (_f_votes._dirty)
            field2Val["votes"] = 
                toSQLString(_f_votes._base);
        if (_f_nshares._dirty)
            field2Val["nshares"] = 
                toSQLString(_f_nshares._base);
        if (_f_float_date._dirty)
            field2Val["float_date"] = 
                toSQLString(_f_float_date._base);
        if (_f_mul_factor._dirty)
            field2Val["mul_factor"] = 
                toSQLString(_f_mul_factor._base);
        if (_f_note._dirty)
            field2Val["note"] = 
                toSQLString(_f_note._base);

        field2Val["created_at"] = "NOW()";

        if (_f_updated_at._dirty)
            field2Val["updated_at"] = 
                toSQLString(_f_updated_at._base);
        if (_f_latest_price._dirty)
            field2Val["latest_price"] = 
                toSQLString(_f_latest_price._base);
        if (_f_primary_class._dirty)
            field2Val["primary_class"] = 
                toSQLString(_f_primary_class._base);

        
        if (DBFace::instance()->
                insert("share_classes", field2Val,
                       _f_id._base.serialization()))
        {
            _f_id._dirty = false;
            _f_stock_id._dirty = false;
            _f_ticker._dirty = false;
            _f_sclass._dirty = false;
            _f_votes._dirty = false;
            _f_nshares._dirty = false;
            _f_float_date._dirty = false;
            _f_mul_factor._dirty = false;
            _f_note._dirty = false;
            _f_created_at._dirty = false;
            _f_updated_at._dirty = false;
            _f_latest_price._dirty = false;
            _f_primary_class._dirty = false;
            return true;
        }
        else
            return false;
                                   
    }
private:
    Field<I_ShareClass> _f_id;
    F_Fixnum _f_stock_id;
    F_String _f_ticker;
    F_String _f_sclass;
    F_Fixnum _f_votes;
    F_String _f_nshares;
    F_Date _f_float_date;
    F_BigDecimal _f_mul_factor;
    F_String _f_note;
    F_Time _f_created_at;
    F_Time _f_updated_at;
    F_BigDecimal _f_latest_price;
    F_Object _f_primary_class;

    friend class T_ShareClass;
};

} //namespace DMMM
#endif //O_SHARECLASS
