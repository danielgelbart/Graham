#ifndef O_OWNEDSTOCK
#define O_OWNEDSTOCK

#include <map>
#include <string>
#include "dmmm_utils.hpp"
#include "dmmm_dbface.h"
#include "dmmm_identifiers.hpp"
#include "dmmm_fields.hpp"
#include "dmmm_comparators.hpp"



namespace DMMM {

class O_OwnedStock{
public:

    O_OwnedStock() {}
    O_OwnedStock(const I_Stock& parent_id) 
        : _f_stock_id(parent_id)
        {}
    O_OwnedStock(const I_Portfolio& parent_id) 
        : _f_portfolio_id(parent_id)
        {}

O_OwnedStock(const I_OwnedStock& id) 
        : _f_id(id)
        {}


    const Field<I_OwnedStock>::Base& _id() const { 
        return _f_id._base; 
    }
    Field<I_OwnedStock>::Base& _id() { 
        _f_id._dirty = true; 
        return _f_id._base; 
    }
    const F_Fixnum::Base& _portfolio_id() const { 
        return _f_portfolio_id._base; 
    }
    F_Fixnum::Base& _portfolio_id() { 
        _f_portfolio_id._dirty = true; 
        return _f_portfolio_id._base; 
    }
    const F_Fixnum::Base& _stock_id() const { 
        return _f_stock_id._base; 
    }
    F_Fixnum::Base& _stock_id() { 
        _f_stock_id._dirty = true; 
        return _f_stock_id._base; 
    }
    const F_Fixnum::Base& _shares() const { 
        return _f_shares._base; 
    }
    F_Fixnum::Base& _shares() { 
        _f_shares._dirty = true; 
        return _f_shares._base; 
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
        if (_f_portfolio_id._dirty)
            field2Val["portfolio_id"] = 
                toSQLString(_f_portfolio_id._base);
        if (_f_stock_id._dirty)
            field2Val["stock_id"] = 
                toSQLString(_f_stock_id._base);
        if (_f_shares._dirty)
            field2Val["shares"] = 
                toSQLString(_f_shares._base);
        if (_f_created_at._dirty)
            field2Val["created_at"] = 
                toSQLString(_f_created_at._base);

        field2Val["updated_at"] = "NOW()";

        std::string where =
            "id=" + toSQLString(_f_id._base);
        if (DBFace::instance()->update("owned_stocks", 
                                       field2Val, where))
        {
            _f_id._dirty = false;
            _f_portfolio_id._dirty = false;
            _f_stock_id._dirty = false;
            _f_shares._dirty = false;
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
        if (_f_portfolio_id._dirty)
            field2Val["portfolio_id"] = 
                toSQLString(_f_portfolio_id._base);
        if (_f_stock_id._dirty)
            field2Val["stock_id"] = 
                toSQLString(_f_stock_id._base);
        if (_f_shares._dirty)
            field2Val["shares"] = 
                toSQLString(_f_shares._base);

        field2Val["created_at"] = "NOW()";

        if (_f_updated_at._dirty)
            field2Val["updated_at"] = 
                toSQLString(_f_updated_at._base);

        
        if (DBFace::instance()->
                insert("owned_stocks", field2Val,
                       _f_id._base.serialization()))
        {
            _f_id._dirty = false;
            _f_portfolio_id._dirty = false;
            _f_stock_id._dirty = false;
            _f_shares._dirty = false;
            _f_created_at._dirty = false;
            _f_updated_at._dirty = false;
            return true;
        }
        else
            return false;
                                   
    }
private:
    Field<I_OwnedStock> _f_id;
    F_Fixnum _f_portfolio_id;
    F_Fixnum _f_stock_id;
    F_Fixnum _f_shares;
    F_Time _f_created_at;
    F_Time _f_updated_at;

    friend class T_OwnedStock;
};

} //namespace DMMM
#endif //O_OWNEDSTOCK
