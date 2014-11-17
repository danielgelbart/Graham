#ifndef O_STOCK
#define O_STOCK

#include <map>
#include <string>
#include "dmmm_utils.hpp"
#include "dmmm_dbface.h"
#include "dmmm_identifiers.hpp"
#include "dmmm_fields.hpp"
#include "dmmm_comparators.hpp"
#include "T_Dividend.hpp"
#include "T_Ep.hpp"
#include "T_Numshare.hpp"
#include "T_BalanceSheet.hpp"



namespace DMMM {

class O_Stock{
public:

    O_Stock() {}

O_Stock(const I_Stock& id) 
        : _f_id(id)
        {}


    std::vector<O_Dividend> _dividends() const
    {
        T_Dividend table;
        return table.select(table._stock_id() == _id());   
    }
    std::vector<O_Dividend> 
        _dividends(const T_Dividend::Condition& c) const
    {
        T_Dividend table(c);
        return table.select(table._stock_id() == _id());   
    }

    std::vector<O_Ep> _eps() const
    {
        T_Ep table;
        return table.select(table._stock_id() == _id());   
    }
    std::vector<O_Ep> 
        _eps(const T_Ep::Condition& c) const
    {
        T_Ep table(c);
        return table.select(table._stock_id() == _id());   
    }

    std::vector<O_Numshare> _numshares() const
    {
        T_Numshare table;
        return table.select(table._stock_id() == _id());   
    }
    std::vector<O_Numshare> 
        _numshares(const T_Numshare::Condition& c) const
    {
        T_Numshare table(c);
        return table.select(table._stock_id() == _id());   
    }

    std::vector<O_BalanceSheet> _balance_sheets() const
    {
        T_BalanceSheet table;
        return table.select(table._stock_id() == _id());   
    }
    std::vector<O_BalanceSheet> 
        _balance_sheets(const T_BalanceSheet::Condition& c) const
    {
        T_BalanceSheet table(c);
        return table.select(table._stock_id() == _id());   
    }

    const Field<I_Stock>::Base& _id() const { 
        return _f_id._base; 
    }
    Field<I_Stock>::Base& _id() { 
        _f_id._dirty = true; 
        return _f_id._base; 
    }
    const F_String::Base& _name() const { 
        return _f_name._base; 
    }
    F_String::Base& _name() { 
        _f_name._dirty = true; 
        return _f_name._base; 
    }
    const F_String::Base& _ticker() const { 
        return _f_ticker._base; 
    }
    F_String::Base& _ticker() { 
        _f_ticker._dirty = true; 
        return _f_ticker._base; 
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
    const F_BigDecimal::Base& _ttm_eps() const { 
        return _f_ttm_eps._base; 
    }
    F_BigDecimal::Base& _ttm_eps() { 
        _f_ttm_eps._dirty = true; 
        return _f_ttm_eps._base; 
    }
    const F_BigDecimal::Base& _book_value_per_share() const { 
        return _f_book_value_per_share._base; 
    }
    F_BigDecimal::Base& _book_value_per_share() { 
        _f_book_value_per_share._dirty = true; 
        return _f_book_value_per_share._base; 
    }
    const F_Date::Base& _fiscal_year_end() const { 
        return _f_fiscal_year_end._base; 
    }
    F_Date::Base& _fiscal_year_end() { 
        _f_fiscal_year_end._dirty = true; 
        return _f_fiscal_year_end._base; 
    }
    const F_Fixnum::Base& _dividends_per_year() const { 
        return _f_dividends_per_year._base; 
    }
    F_Fixnum::Base& _dividends_per_year() { 
        _f_dividends_per_year._dirty = true; 
        return _f_dividends_per_year._base; 
    }
    const F_BigDecimal::Base& _latest_price() const { 
        return _f_latest_price._base; 
    }
    F_BigDecimal::Base& _latest_price() { 
        _f_latest_price._dirty = true; 
        return _f_latest_price._base; 
    }
    const F_String::Base& _market_cap() const { 
        return _f_market_cap._base; 
    }
    F_String::Base& _market_cap() { 
        _f_market_cap._dirty = true; 
        return _f_market_cap._base; 
    }
    const F_BigDecimal::Base& _ttm_div() const { 
        return _f_ttm_div._base; 
    }
    F_BigDecimal::Base& _ttm_div() { 
        _f_ttm_div._dirty = true; 
        return _f_ttm_div._base; 
    }
    const F_BigDecimal::Base& _yield() const { 
        return _f_yield._base; 
    }
    F_BigDecimal::Base& _yield() { 
        _f_yield._dirty = true; 
        return _f_yield._base; 
    }
    const F_Object::Base& _listed() const { 
        return _f_listed._base; 
    }
    F_Object::Base& _listed() { 
        _f_listed._dirty = true; 
        return _f_listed._base; 
    }
    const F_Object::Base& _has_currant_ratio() const { 
        return _f_has_currant_ratio._base; 
    }
    F_Object::Base& _has_currant_ratio() { 
        _f_has_currant_ratio._dirty = true; 
        return _f_has_currant_ratio._base; 
    }
    const F_String::Base& _mark() const { 
        return _f_mark._base; 
    }
    F_String::Base& _mark() { 
        _f_mark._dirty = true; 
        return _f_mark._base; 
    }
    const F_Fixnum::Base& _cik() const { 
        return _f_cik._base; 
    }
    F_Fixnum::Base& _cik() { 
        _f_cik._dirty = true; 
        return _f_cik._base; 
    }

    bool update(){
        std::map<std::string, std::string> field2Val;
        if (_f_id._dirty)
            field2Val["id"] = 
                toSQLString(_f_id._base);
        if (_f_name._dirty)
            field2Val["name"] = 
                toSQLString(_f_name._base);
        if (_f_ticker._dirty)
            field2Val["ticker"] = 
                toSQLString(_f_ticker._base);
        if (_f_created_at._dirty)
            field2Val["created_at"] = 
                toSQLString(_f_created_at._base);

        field2Val["updated_at"] = "NOW()";

        if (_f_ttm_eps._dirty)
            field2Val["ttm_eps"] = 
                toSQLString(_f_ttm_eps._base);
        if (_f_book_value_per_share._dirty)
            field2Val["book_value_per_share"] = 
                toSQLString(_f_book_value_per_share._base);
        if (_f_fiscal_year_end._dirty)
            field2Val["fiscal_year_end"] = 
                toSQLString(_f_fiscal_year_end._base);
        if (_f_dividends_per_year._dirty)
            field2Val["dividends_per_year"] = 
                toSQLString(_f_dividends_per_year._base);
        if (_f_latest_price._dirty)
            field2Val["latest_price"] = 
                toSQLString(_f_latest_price._base);
        if (_f_market_cap._dirty)
            field2Val["market_cap"] = 
                toSQLString(_f_market_cap._base);
        if (_f_ttm_div._dirty)
            field2Val["ttm_div"] = 
                toSQLString(_f_ttm_div._base);
        if (_f_yield._dirty)
            field2Val["yield"] = 
                toSQLString(_f_yield._base);
        if (_f_listed._dirty)
            field2Val["listed"] = 
                toSQLString(_f_listed._base);
        if (_f_has_currant_ratio._dirty)
            field2Val["has_currant_ratio"] = 
                toSQLString(_f_has_currant_ratio._base);
        if (_f_mark._dirty)
            field2Val["mark"] = 
                toSQLString(_f_mark._base);
        if (_f_cik._dirty)
            field2Val["cik"] = 
                toSQLString(_f_cik._base);
        std::string where =
            "id=" + toSQLString(_f_id._base);
        if (DBFace::instance()->update("stocks", 
                                       field2Val, where))
        {
            _f_id._dirty = false;
            _f_name._dirty = false;
            _f_ticker._dirty = false;
            _f_created_at._dirty = false;
            _f_updated_at._dirty = false;
            _f_ttm_eps._dirty = false;
            _f_book_value_per_share._dirty = false;
            _f_fiscal_year_end._dirty = false;
            _f_dividends_per_year._dirty = false;
            _f_latest_price._dirty = false;
            _f_market_cap._dirty = false;
            _f_ttm_div._dirty = false;
            _f_yield._dirty = false;
            _f_listed._dirty = false;
            _f_has_currant_ratio._dirty = false;
            _f_mark._dirty = false;
            _f_cik._dirty = false;
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
        if (_f_name._dirty)
            field2Val["name"] = 
                toSQLString(_f_name._base);
        if (_f_ticker._dirty)
            field2Val["ticker"] = 
                toSQLString(_f_ticker._base);

        field2Val["created_at"] = "NOW()";

        if (_f_updated_at._dirty)
            field2Val["updated_at"] = 
                toSQLString(_f_updated_at._base);
        if (_f_ttm_eps._dirty)
            field2Val["ttm_eps"] = 
                toSQLString(_f_ttm_eps._base);
        if (_f_book_value_per_share._dirty)
            field2Val["book_value_per_share"] = 
                toSQLString(_f_book_value_per_share._base);
        if (_f_fiscal_year_end._dirty)
            field2Val["fiscal_year_end"] = 
                toSQLString(_f_fiscal_year_end._base);
        if (_f_dividends_per_year._dirty)
            field2Val["dividends_per_year"] = 
                toSQLString(_f_dividends_per_year._base);
        if (_f_latest_price._dirty)
            field2Val["latest_price"] = 
                toSQLString(_f_latest_price._base);
        if (_f_market_cap._dirty)
            field2Val["market_cap"] = 
                toSQLString(_f_market_cap._base);
        if (_f_ttm_div._dirty)
            field2Val["ttm_div"] = 
                toSQLString(_f_ttm_div._base);
        if (_f_yield._dirty)
            field2Val["yield"] = 
                toSQLString(_f_yield._base);
        if (_f_listed._dirty)
            field2Val["listed"] = 
                toSQLString(_f_listed._base);
        if (_f_has_currant_ratio._dirty)
            field2Val["has_currant_ratio"] = 
                toSQLString(_f_has_currant_ratio._base);
        if (_f_mark._dirty)
            field2Val["mark"] = 
                toSQLString(_f_mark._base);
        if (_f_cik._dirty)
            field2Val["cik"] = 
                toSQLString(_f_cik._base);

        
        if (DBFace::instance()->
                insert("stocks", field2Val,
                       _f_id._base.serialization()))
        {
            _f_id._dirty = false;
            _f_name._dirty = false;
            _f_ticker._dirty = false;
            _f_created_at._dirty = false;
            _f_updated_at._dirty = false;
            _f_ttm_eps._dirty = false;
            _f_book_value_per_share._dirty = false;
            _f_fiscal_year_end._dirty = false;
            _f_dividends_per_year._dirty = false;
            _f_latest_price._dirty = false;
            _f_market_cap._dirty = false;
            _f_ttm_div._dirty = false;
            _f_yield._dirty = false;
            _f_listed._dirty = false;
            _f_has_currant_ratio._dirty = false;
            _f_mark._dirty = false;
            _f_cik._dirty = false;
            return true;
        }
        else
            return false;
                                   
    }
private:
    Field<I_Stock> _f_id;
    F_String _f_name;
    F_String _f_ticker;
    F_Time _f_created_at;
    F_Time _f_updated_at;
    F_BigDecimal _f_ttm_eps;
    F_BigDecimal _f_book_value_per_share;
    F_Date _f_fiscal_year_end;
    F_Fixnum _f_dividends_per_year;
    F_BigDecimal _f_latest_price;
    F_String _f_market_cap;
    F_BigDecimal _f_ttm_div;
    F_BigDecimal _f_yield;
    F_Object _f_listed;
    F_Object _f_has_currant_ratio;
    F_String _f_mark;
    F_Fixnum _f_cik;

    friend class T_Stock;
};

} //namespace DMMM
#endif //O_STOCK
