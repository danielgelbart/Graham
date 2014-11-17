#ifndef O_EP
#define O_EP

#include <map>
#include <string>
#include "dmmm_utils.hpp"
#include "dmmm_dbface.h"
#include "dmmm_identifiers.hpp"
#include "dmmm_fields.hpp"
#include "dmmm_comparators.hpp"



namespace DMMM {

class O_Ep{
public:

    O_Ep() {}
    O_Ep(const I_Stock& parent_id) 
    : _f_stock_id(parent_id)
        {}

O_Ep(const I_Ep& id) 
        : _f_id(id)
        {}


    const Field<I_Ep>::Base& _id() const { 
        return _f_id._base; 
    }
    Field<I_Ep>::Base& _id() { 
        _f_id._dirty = true; 
        return _f_id._base; 
    }
    const F_Fixnum::Base& _year() const { 
        return _f_year._base; 
    }
    F_Fixnum::Base& _year() { 
        _f_year._dirty = true; 
        return _f_year._base; 
    }
    const F_String::Base& _source() const { 
        return _f_source._base; 
    }
    F_String::Base& _source() { 
        _f_source._dirty = true; 
        return _f_source._base; 
    }
    const F_BigDecimal::Base& _eps() const { 
        return _f_eps._base; 
    }
    F_BigDecimal::Base& _eps() { 
        _f_eps._dirty = true; 
        return _f_eps._base; 
    }
    const F_Fixnum::Base& _stock_id() const { 
        return _f_stock_id._base; 
    }
    F_Fixnum::Base& _stock_id() { 
        _f_stock_id._dirty = true; 
        return _f_stock_id._base; 
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
    const F_String::Base& _revenue() const { 
        return _f_revenue._base; 
    }
    F_String::Base& _revenue() { 
        _f_revenue._dirty = true; 
        return _f_revenue._base; 
    }
    const F_String::Base& _net_income() const { 
        return _f_net_income._base; 
    }
    F_String::Base& _net_income() { 
        _f_net_income._dirty = true; 
        return _f_net_income._base; 
    }
    const F_Fixnum::Base& _quarter() const { 
        return _f_quarter._base; 
    }
    F_Fixnum::Base& _quarter() { 
        _f_quarter._dirty = true; 
        return _f_quarter._base; 
    }

    bool update(){
        std::map<std::string, std::string> field2Val;
        if (_f_id._dirty)
            field2Val["id"] = 
                toSQLString(_f_id._base);
        if (_f_year._dirty)
            field2Val["year"] = 
                toSQLString(_f_year._base);
        if (_f_source._dirty)
            field2Val["source"] = 
                toSQLString(_f_source._base);
        if (_f_eps._dirty)
            field2Val["eps"] = 
                toSQLString(_f_eps._base);
        if (_f_stock_id._dirty)
            field2Val["stock_id"] = 
                toSQLString(_f_stock_id._base);
        if (_f_created_at._dirty)
            field2Val["created_at"] = 
                toSQLString(_f_created_at._base);

        field2Val["updated_at"] = "NOW()";

        if (_f_revenue._dirty)
            field2Val["revenue"] = 
                toSQLString(_f_revenue._base);
        if (_f_net_income._dirty)
            field2Val["net_income"] = 
                toSQLString(_f_net_income._base);
        if (_f_quarter._dirty)
            field2Val["quarter"] = 
                toSQLString(_f_quarter._base);
        std::string where =
            "id=" + toSQLString(_f_id._base);
        if (DBFace::instance()->update("eps", 
                                       field2Val, where))
        {
            _f_id._dirty = false;
            _f_year._dirty = false;
            _f_source._dirty = false;
            _f_eps._dirty = false;
            _f_stock_id._dirty = false;
            _f_created_at._dirty = false;
            _f_updated_at._dirty = false;
            _f_revenue._dirty = false;
            _f_net_income._dirty = false;
            _f_quarter._dirty = false;
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
        if (_f_year._dirty)
            field2Val["year"] = 
                toSQLString(_f_year._base);
        if (_f_source._dirty)
            field2Val["source"] = 
                toSQLString(_f_source._base);
        if (_f_eps._dirty)
            field2Val["eps"] = 
                toSQLString(_f_eps._base);
        if (_f_stock_id._dirty)
            field2Val["stock_id"] = 
                toSQLString(_f_stock_id._base);

        field2Val["created_at"] = "NOW()";

        if (_f_updated_at._dirty)
            field2Val["updated_at"] = 
                toSQLString(_f_updated_at._base);
        if (_f_revenue._dirty)
            field2Val["revenue"] = 
                toSQLString(_f_revenue._base);
        if (_f_net_income._dirty)
            field2Val["net_income"] = 
                toSQLString(_f_net_income._base);
        if (_f_quarter._dirty)
            field2Val["quarter"] = 
                toSQLString(_f_quarter._base);

        
        if (DBFace::instance()->
                insert("eps", field2Val,
                       _f_id._base.serialization()))
        {
            _f_id._dirty = false;
            _f_year._dirty = false;
            _f_source._dirty = false;
            _f_eps._dirty = false;
            _f_stock_id._dirty = false;
            _f_created_at._dirty = false;
            _f_updated_at._dirty = false;
            _f_revenue._dirty = false;
            _f_net_income._dirty = false;
            _f_quarter._dirty = false;
            return true;
        }
        else
            return false;
                                   
    }
private:
    Field<I_Ep> _f_id;
    F_Fixnum _f_year;
    F_String _f_source;
    F_BigDecimal _f_eps;
    F_Fixnum _f_stock_id;
    F_Time _f_created_at;
    F_Time _f_updated_at;
    F_String _f_revenue;
    F_String _f_net_income;
    F_Fixnum _f_quarter;

    friend class T_Ep;
};

} //namespace DMMM
#endif //O_EP
