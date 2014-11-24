#ifndef O_BALANCESHEET
#define O_BALANCESHEET

#include <map>
#include <string>
#include "dmmm_utils.hpp"
#include "dmmm_dbface.h"
#include "dmmm_identifiers.hpp"
#include "dmmm_fields.hpp"
#include "dmmm_comparators.hpp"



namespace DMMM {

class O_BalanceSheet{
public:

    O_BalanceSheet() {}
    O_BalanceSheet(const I_Stock& parent_id) 
        : _f_stock_id(parent_id)
        {}

O_BalanceSheet(const I_BalanceSheet& id) 
        : _f_id(id)
        {}


    const Field<I_BalanceSheet>::Base& _id() const { 
        return _f_id._base; 
    }
    Field<I_BalanceSheet>::Base& _id() { 
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
    const F_Fixnum::Base& _year() const { 
        return _f_year._base; 
    }
    F_Fixnum::Base& _year() { 
        _f_year._dirty = true; 
        return _f_year._base; 
    }
    const F_String::Base& _current_assets() const { 
        return _f_current_assets._base; 
    }
    F_String::Base& _current_assets() { 
        _f_current_assets._dirty = true; 
        return _f_current_assets._base; 
    }
    const F_String::Base& _total_assets() const { 
        return _f_total_assets._base; 
    }
    F_String::Base& _total_assets() { 
        _f_total_assets._dirty = true; 
        return _f_total_assets._base; 
    }
    const F_String::Base& _current_liabilities() const { 
        return _f_current_liabilities._base; 
    }
    F_String::Base& _current_liabilities() { 
        _f_current_liabilities._dirty = true; 
        return _f_current_liabilities._base; 
    }
    const F_String::Base& _total_liabilities() const { 
        return _f_total_liabilities._base; 
    }
    F_String::Base& _total_liabilities() { 
        _f_total_liabilities._dirty = true; 
        return _f_total_liabilities._base; 
    }
    const F_String::Base& _long_term_debt() const { 
        return _f_long_term_debt._base; 
    }
    F_String::Base& _long_term_debt() { 
        _f_long_term_debt._dirty = true; 
        return _f_long_term_debt._base; 
    }
    const F_String::Base& _book_value() const { 
        return _f_book_value._base; 
    }
    F_String::Base& _book_value() { 
        _f_book_value._dirty = true; 
        return _f_book_value._base; 
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
    const F_String::Base& _net_tangible_assets() const { 
        return _f_net_tangible_assets._base; 
    }
    F_String::Base& _net_tangible_assets() { 
        _f_net_tangible_assets._dirty = true; 
        return _f_net_tangible_assets._base; 
    }
    const F_String::Base& _total_sales() const { 
        return _f_total_sales._base; 
    }
    F_String::Base& _total_sales() { 
        _f_total_sales._dirty = true; 
        return _f_total_sales._base; 
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
        if (_f_stock_id._dirty)
            field2Val["stock_id"] = 
                toSQLString(_f_stock_id._base);
        if (_f_year._dirty)
            field2Val["year"] = 
                toSQLString(_f_year._base);
        if (_f_current_assets._dirty)
            field2Val["current_assets"] = 
                toSQLString(_f_current_assets._base);
        if (_f_total_assets._dirty)
            field2Val["total_assets"] = 
                toSQLString(_f_total_assets._base);
        if (_f_current_liabilities._dirty)
            field2Val["current_liabilities"] = 
                toSQLString(_f_current_liabilities._base);
        if (_f_total_liabilities._dirty)
            field2Val["total_liabilities"] = 
                toSQLString(_f_total_liabilities._base);
        if (_f_long_term_debt._dirty)
            field2Val["long_term_debt"] = 
                toSQLString(_f_long_term_debt._base);
        if (_f_book_value._dirty)
            field2Val["book_value"] = 
                toSQLString(_f_book_value._base);
        if (_f_created_at._dirty)
            field2Val["created_at"] = 
                toSQLString(_f_created_at._base);

        field2Val["updated_at"] = "NOW()";

        if (_f_net_tangible_assets._dirty)
            field2Val["net_tangible_assets"] = 
                toSQLString(_f_net_tangible_assets._base);
        if (_f_total_sales._dirty)
            field2Val["total_sales"] = 
                toSQLString(_f_total_sales._base);
        if (_f_quarter._dirty)
            field2Val["quarter"] = 
                toSQLString(_f_quarter._base);
        std::string where =
            "id=" + toSQLString(_f_id._base);
        if (DBFace::instance()->update("balance_sheets", 
                                       field2Val, where))
        {
            _f_id._dirty = false;
            _f_stock_id._dirty = false;
            _f_year._dirty = false;
            _f_current_assets._dirty = false;
            _f_total_assets._dirty = false;
            _f_current_liabilities._dirty = false;
            _f_total_liabilities._dirty = false;
            _f_long_term_debt._dirty = false;
            _f_book_value._dirty = false;
            _f_created_at._dirty = false;
            _f_updated_at._dirty = false;
            _f_net_tangible_assets._dirty = false;
            _f_total_sales._dirty = false;
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
        if (_f_stock_id._dirty)
            field2Val["stock_id"] = 
                toSQLString(_f_stock_id._base);
        if (_f_year._dirty)
            field2Val["year"] = 
                toSQLString(_f_year._base);
        if (_f_current_assets._dirty)
            field2Val["current_assets"] = 
                toSQLString(_f_current_assets._base);
        if (_f_total_assets._dirty)
            field2Val["total_assets"] = 
                toSQLString(_f_total_assets._base);
        if (_f_current_liabilities._dirty)
            field2Val["current_liabilities"] = 
                toSQLString(_f_current_liabilities._base);
        if (_f_total_liabilities._dirty)
            field2Val["total_liabilities"] = 
                toSQLString(_f_total_liabilities._base);
        if (_f_long_term_debt._dirty)
            field2Val["long_term_debt"] = 
                toSQLString(_f_long_term_debt._base);
        if (_f_book_value._dirty)
            field2Val["book_value"] = 
                toSQLString(_f_book_value._base);

        field2Val["created_at"] = "NOW()";

        if (_f_updated_at._dirty)
            field2Val["updated_at"] = 
                toSQLString(_f_updated_at._base);
        if (_f_net_tangible_assets._dirty)
            field2Val["net_tangible_assets"] = 
                toSQLString(_f_net_tangible_assets._base);
        if (_f_total_sales._dirty)
            field2Val["total_sales"] = 
                toSQLString(_f_total_sales._base);
        if (_f_quarter._dirty)
            field2Val["quarter"] = 
                toSQLString(_f_quarter._base);

        
        if (DBFace::instance()->
                insert("balance_sheets", field2Val,
                       _f_id._base.serialization()))
        {
            _f_id._dirty = false;
            _f_stock_id._dirty = false;
            _f_year._dirty = false;
            _f_current_assets._dirty = false;
            _f_total_assets._dirty = false;
            _f_current_liabilities._dirty = false;
            _f_total_liabilities._dirty = false;
            _f_long_term_debt._dirty = false;
            _f_book_value._dirty = false;
            _f_created_at._dirty = false;
            _f_updated_at._dirty = false;
            _f_net_tangible_assets._dirty = false;
            _f_total_sales._dirty = false;
            _f_quarter._dirty = false;
            return true;
        }
        else
            return false;
                                   
    }
private:
    Field<I_BalanceSheet> _f_id;
    F_Fixnum _f_stock_id;
    F_Fixnum _f_year;
    F_String _f_current_assets;
    F_String _f_total_assets;
    F_String _f_current_liabilities;
    F_String _f_total_liabilities;
    F_String _f_long_term_debt;
    F_String _f_book_value;
    F_Time _f_created_at;
    F_Time _f_updated_at;
    F_String _f_net_tangible_assets;
    F_String _f_total_sales;
    F_Fixnum _f_quarter;

    friend class T_BalanceSheet;
};

} //namespace DMMM
#endif //O_BALANCESHEET
