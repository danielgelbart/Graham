#ifndef O_PORTFOLIO
#define O_PORTFOLIO

#include <map>
#include <string>
#include "dmmm_utils.hpp"
#include "dmmm_dbface.h"
#include "dmmm_identifiers.hpp"
#include "dmmm_fields.hpp"
#include "dmmm_comparators.hpp"
#include "T_Transaction.hpp"
#include "T_OwnedStock.hpp"



namespace DMMM {

class O_Portfolio{
public:

    O_Portfolio() {}
    O_Portfolio(const I_User& parent_id) 
        : _f_user_id(parent_id)
        {}

O_Portfolio(const I_Portfolio& id) 
        : _f_id(id)
        {}


    std::vector<O_Transaction> _transactions() const
    {
        T_Transaction table;
        return table.select(table._portfolio_id() == _id());   
    }
    std::vector<O_Transaction> 
        _transactions(const T_Transaction::Condition& c) const
    {
        T_Transaction table(c);
        return table.select(table._portfolio_id() == _id());   
    }

    std::vector<O_OwnedStock> _owned_stocks() const
    {
        T_OwnedStock table;
        return table.select(table._portfolio_id() == _id());   
    }
    std::vector<O_OwnedStock> 
        _owned_stocks(const T_OwnedStock::Condition& c) const
    {
        T_OwnedStock table(c);
        return table.select(table._portfolio_id() == _id());   
    }

    const Field<I_Portfolio>::Base& _id() const { 
        return _f_id._base; 
    }
    Field<I_Portfolio>::Base& _id() { 
        _f_id._dirty = true; 
        return _f_id._base; 
    }
    const F_Fixnum::Base& _user_id() const { 
        return _f_user_id._base; 
    }
    F_Fixnum::Base& _user_id() { 
        _f_user_id._dirty = true; 
        return _f_user_id._base; 
    }
    const F_String::Base& _name() const { 
        return _f_name._base; 
    }
    F_String::Base& _name() { 
        _f_name._dirty = true; 
        return _f_name._base; 
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
        if (_f_user_id._dirty)
            field2Val["user_id"] = 
                toSQLString(_f_user_id._base);
        if (_f_name._dirty)
            field2Val["name"] = 
                toSQLString(_f_name._base);
        if (_f_created_at._dirty)
            field2Val["created_at"] = 
                toSQLString(_f_created_at._base);

        field2Val["updated_at"] = "NOW()";

        std::string where =
            "id=" + toSQLString(_f_id._base);
        if (DBFace::instance()->update("portfolios", 
                                       field2Val, where))
        {
            _f_id._dirty = false;
            _f_user_id._dirty = false;
            _f_name._dirty = false;
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
        if (_f_user_id._dirty)
            field2Val["user_id"] = 
                toSQLString(_f_user_id._base);
        if (_f_name._dirty)
            field2Val["name"] = 
                toSQLString(_f_name._base);

        field2Val["created_at"] = "NOW()";

        if (_f_updated_at._dirty)
            field2Val["updated_at"] = 
                toSQLString(_f_updated_at._base);

        
        if (DBFace::instance()->
                insert("portfolios", field2Val,
                       _f_id._base.serialization()))
        {
            _f_id._dirty = false;
            _f_user_id._dirty = false;
            _f_name._dirty = false;
            _f_created_at._dirty = false;
            _f_updated_at._dirty = false;
            return true;
        }
        else
            return false;
                                   
    }
private:
    Field<I_Portfolio> _f_id;
    F_Fixnum _f_user_id;
    F_String _f_name;
    F_Time _f_created_at;
    F_Time _f_updated_at;

    friend class T_Portfolio;
};

} //namespace DMMM
#endif //O_PORTFOLIO
