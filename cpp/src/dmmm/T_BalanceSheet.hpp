
#ifndef T_BALANCESHEET
#define T_BALANCESHEET
#include "O_BalanceSheet.hpp"

namespace DMMM {

class T_BalanceSheet{
public:

    struct Condition{
        bool nil() const { return _cond.size() == 0; }
        std::string _cond;
    };

    T_BalanceSheet() 
    {
        _tables.push_back("balance_sheets");
    }
    T_BalanceSheet(const Condition& c) 
        : _constraint(c)
    {
        _tables.push_back("balance_sheets");
    }

 
    T_BalanceSheet(const I_Stock& parentId)
    {
        _tables.push_back("balance_sheets");
        
        _constraint._cond = "(balance_sheets.stock_id = " + parentId.to_s() + ")";
    } 
    
    struct E_id{
        E_id() 
        {  
            _field = "balance_sheets.id";
        }
        std::string _field;
        typedef T_BalanceSheet::Condition ConditionType;
        typedef Field<I_BalanceSheet>::Base ComparerType;
    };

    static E_id _id(){ 
        return E_id();
    }
    struct E_stock_id{
        E_stock_id() 
        {  
            _field = "balance_sheets.stock_id";
        }
        std::string _field;
        typedef T_BalanceSheet::Condition ConditionType;
        typedef F_Fixnum::Base ComparerType;
    };

    static E_stock_id _stock_id(){ 
        return E_stock_id();
    }
    struct E_year{
        E_year() 
        {  
            _field = "balance_sheets.year";
        }
        std::string _field;
        typedef T_BalanceSheet::Condition ConditionType;
        typedef F_Fixnum::Base ComparerType;
    };

    static E_year _year(){ 
        return E_year();
    }
    struct E_current_assets{
        E_current_assets() 
        {  
            _field = "balance_sheets.current_assets";
        }
        std::string _field;
        typedef T_BalanceSheet::Condition ConditionType;
        typedef F_String::Base ComparerType;
    };

    static E_current_assets _current_assets(){ 
        return E_current_assets();
    }
    struct E_total_assets{
        E_total_assets() 
        {  
            _field = "balance_sheets.total_assets";
        }
        std::string _field;
        typedef T_BalanceSheet::Condition ConditionType;
        typedef F_String::Base ComparerType;
    };

    static E_total_assets _total_assets(){ 
        return E_total_assets();
    }
    struct E_current_liabilities{
        E_current_liabilities() 
        {  
            _field = "balance_sheets.current_liabilities";
        }
        std::string _field;
        typedef T_BalanceSheet::Condition ConditionType;
        typedef F_String::Base ComparerType;
    };

    static E_current_liabilities _current_liabilities(){ 
        return E_current_liabilities();
    }
    struct E_total_liabilities{
        E_total_liabilities() 
        {  
            _field = "balance_sheets.total_liabilities";
        }
        std::string _field;
        typedef T_BalanceSheet::Condition ConditionType;
        typedef F_String::Base ComparerType;
    };

    static E_total_liabilities _total_liabilities(){ 
        return E_total_liabilities();
    }
    struct E_long_term_debt{
        E_long_term_debt() 
        {  
            _field = "balance_sheets.long_term_debt";
        }
        std::string _field;
        typedef T_BalanceSheet::Condition ConditionType;
        typedef F_String::Base ComparerType;
    };

    static E_long_term_debt _long_term_debt(){ 
        return E_long_term_debt();
    }
    struct E_book_value{
        E_book_value() 
        {  
            _field = "balance_sheets.book_value";
        }
        std::string _field;
        typedef T_BalanceSheet::Condition ConditionType;
        typedef F_String::Base ComparerType;
    };

    static E_book_value _book_value(){ 
        return E_book_value();
    }
    struct E_created_at{
        E_created_at() 
        {  
            _field = "balance_sheets.created_at";
        }
        std::string _field;
        typedef T_BalanceSheet::Condition ConditionType;
        typedef F_Time::Base ComparerType;
    };

    static E_created_at _created_at(){ 
        return E_created_at();
    }
    struct E_updated_at{
        E_updated_at() 
        {  
            _field = "balance_sheets.updated_at";
        }
        std::string _field;
        typedef T_BalanceSheet::Condition ConditionType;
        typedef F_Time::Base ComparerType;
    };

    static E_updated_at _updated_at(){ 
        return E_updated_at();
    }
    struct E_net_tangible_assets{
        E_net_tangible_assets() 
        {  
            _field = "balance_sheets.net_tangible_assets";
        }
        std::string _field;
        typedef T_BalanceSheet::Condition ConditionType;
        typedef F_String::Base ComparerType;
    };

    static E_net_tangible_assets _net_tangible_assets(){ 
        return E_net_tangible_assets();
    }
    struct E_quarter{
        E_quarter() 
        {  
            _field = "balance_sheets.quarter";
        }
        std::string _field;
        typedef T_BalanceSheet::Condition ConditionType;
        typedef F_Fixnum::Base ComparerType;
    };

    static E_quarter _quarter(){ 
        return E_quarter();
    }
    struct E_calculated_bv{
        E_calculated_bv() 
        {  
            _field = "balance_sheets.calculated_bv";
        }
        std::string _field;
        typedef T_BalanceSheet::Condition ConditionType;
        typedef F_Object::Base ComparerType;
    };

    static E_calculated_bv _calculated_bv(){ 
        return E_calculated_bv();
    }
    struct E_calculated_tl{
        E_calculated_tl() 
        {  
            _field = "balance_sheets.calculated_tl";
        }
        std::string _field;
        typedef T_BalanceSheet::Condition ConditionType;
        typedef F_Object::Base ComparerType;
    };

    static E_calculated_tl _calculated_tl(){ 
        return E_calculated_tl();
    }
    

    std::vector<std::string> getFields()
    {
        std::vector<std::string> ret;
        ret.clear();
        ret.push_back("id");
        ret.push_back("stock_id");
        ret.push_back("year");
        ret.push_back("current_assets");
        ret.push_back("total_assets");
        ret.push_back("current_liabilities");
        ret.push_back("total_liabilities");
        ret.push_back("long_term_debt");
        ret.push_back("book_value");
        ret.push_back("created_at");
        ret.push_back("updated_at");
        ret.push_back("net_tangible_assets");
        ret.push_back("quarter");
        ret.push_back("calculated_bv");
        ret.push_back("calculated_tl");
        return ret;
    }

    std::vector<O_BalanceSheet> 
    select(const Condition& c, const std::string& additional)
    {
        std::vector<O_BalanceSheet> ret;
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        std::vector<std::string> fields = getFields();
        QueryRes res;
        DBFace::instance()->select(_tables, fields, c1._cond, 
                                   additional, res);
        ret.resize(res.size());
        for(size_t i = 0; i < res.size(); ++i){
            ret[i]._f_id._base =
                UTILS::fromString<Field<I_BalanceSheet>::Base>(res[i]["id"]);
            ret[i]._f_stock_id._base =
                UTILS::fromString<F_Fixnum::Base>(res[i]["stock_id"]);
            ret[i]._f_year._base =
                UTILS::fromString<F_Fixnum::Base>(res[i]["year"]);
            ret[i]._f_current_assets._base =
                UTILS::fromString<F_String::Base>(res[i]["current_assets"]);
            ret[i]._f_total_assets._base =
                UTILS::fromString<F_String::Base>(res[i]["total_assets"]);
            ret[i]._f_current_liabilities._base =
                UTILS::fromString<F_String::Base>(res[i]["current_liabilities"]);
            ret[i]._f_total_liabilities._base =
                UTILS::fromString<F_String::Base>(res[i]["total_liabilities"]);
            ret[i]._f_long_term_debt._base =
                UTILS::fromString<F_String::Base>(res[i]["long_term_debt"]);
            ret[i]._f_book_value._base =
                UTILS::fromString<F_String::Base>(res[i]["book_value"]);
            ret[i]._f_created_at._base =
                UTILS::fromString<F_Time::Base>(res[i]["created_at"]);
            ret[i]._f_updated_at._base =
                UTILS::fromString<F_Time::Base>(res[i]["updated_at"]);
            ret[i]._f_net_tangible_assets._base =
                UTILS::fromString<F_String::Base>(res[i]["net_tangible_assets"]);
            ret[i]._f_quarter._base =
                UTILS::fromString<F_Fixnum::Base>(res[i]["quarter"]);
            ret[i]._f_calculated_bv._base =
                UTILS::fromString<F_Object::Base>(res[i]["calculated_bv"]);
            ret[i]._f_calculated_tl._base =
                UTILS::fromString<F_Object::Base>(res[i]["calculated_tl"]);
        }
        return ret;
    }

    size_t count(const Condition& c)
    {
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        return DBFace::instance()->count(_tables, c1._cond);
    }

    void erase(const Condition& c)
    {
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        DBFace::instance()->erase(_tables, c1._cond);
    }

    void erase()
    {   
        DBFace::instance()->erase(_tables, _constraint._cond);
    }


    std::vector<O_BalanceSheet> select(const Condition& c)
    {
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        return select(c1, "");
    }

    std::vector<O_BalanceSheet> 
    select(const std::string& additional)
    {
        return select(_constraint, additional);
    }

    std::vector<O_BalanceSheet> select()
    {
        return select(_constraint, "");
    }

    std::pair<O_BalanceSheet, bool> select(const I_BalanceSheet& id){
        return first(E_id() == id);
    }

    std::pair<O_BalanceSheet, bool> first(const Condition& c)
    {
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        std::vector<O_BalanceSheet> r = select(c, "limit 1");
        if(r.size() > 0)
            return std::make_pair(r[0], true);
        else
            return std::make_pair(O_BalanceSheet(), false);
    }

    std::pair<O_BalanceSheet, bool> first(){
        return first(_constraint);
    }

    bool insertAllFields(std::vector<O_BalanceSheet>::const_iterator begin,
                         std::vector<O_BalanceSheet>::const_iterator end)
    {
	std::vector<std::string> fields;
                

        fields.push_back(std::string("stock_id"));
        

        fields.push_back(std::string("year"));
        

        fields.push_back(std::string("current_assets"));
        

        fields.push_back(std::string("total_assets"));
        

        fields.push_back(std::string("current_liabilities"));
        

        fields.push_back(std::string("total_liabilities"));
        

        fields.push_back(std::string("long_term_debt"));
        

        fields.push_back(std::string("book_value"));
        

        fields.push_back(std::string("created_at"));
        

        fields.push_back(std::string("updated_at"));
        

        fields.push_back(std::string("net_tangible_assets"));
        

        fields.push_back(std::string("quarter"));
        

        fields.push_back(std::string("calculated_bv"));
        

        fields.push_back(std::string("calculated_tl"));
	std::vector<std::vector<std::string> > rows;
	for (; begin != end; ++begin){
	    const O_BalanceSheet& r = *begin;
	    std::vector<std::string> row;
                        
            row.push_back(toSQLString(r._stock_id()));
            
            row.push_back(toSQLString(r._year()));
            
            row.push_back(toSQLString(r._current_assets()));
            
            row.push_back(toSQLString(r._total_assets()));
            
            row.push_back(toSQLString(r._current_liabilities()));
            
            row.push_back(toSQLString(r._total_liabilities()));
            
            row.push_back(toSQLString(r._long_term_debt()));
            
            row.push_back(toSQLString(r._book_value()));
            
            row.push_back(toSQLString(r._created_at()));
            
            row.push_back(toSQLString(r._updated_at()));
            
            row.push_back(toSQLString(r._net_tangible_assets()));
            
            row.push_back(toSQLString(r._quarter()));
            
            row.push_back(toSQLString(r._calculated_bv()));
            
            row.push_back(toSQLString(r._calculated_tl()));
	    rows.push_back(row);
	}
        return DBFace::instance()->insert("balance_sheets",
					  fields, rows);
    }

    bool insertDirtyFields(std::vector<O_BalanceSheet>::const_iterator begin,
                           std::vector<O_BalanceSheet>::const_iterator end)
    {
        if (begin == end)
            return true;
        FieldsToRows fields2Rows;
	for (std::vector<O_BalanceSheet>::const_iterator it = begin; 
             it != end; ++it)
        {
            std::vector<std::string> fields;
            std::vector<std::string> row;
                        

            if (it->_f_stock_id._dirty){
                fields.push_back(std::string("stock_id"));
                row.push_back(toSQLString(it->_stock_id()));
            }
            

            if (it->_f_year._dirty){
                fields.push_back(std::string("year"));
                row.push_back(toSQLString(it->_year()));
            }
            

            if (it->_f_current_assets._dirty){
                fields.push_back(std::string("current_assets"));
                row.push_back(toSQLString(it->_current_assets()));
            }
            

            if (it->_f_total_assets._dirty){
                fields.push_back(std::string("total_assets"));
                row.push_back(toSQLString(it->_total_assets()));
            }
            

            if (it->_f_current_liabilities._dirty){
                fields.push_back(std::string("current_liabilities"));
                row.push_back(toSQLString(it->_current_liabilities()));
            }
            

            if (it->_f_total_liabilities._dirty){
                fields.push_back(std::string("total_liabilities"));
                row.push_back(toSQLString(it->_total_liabilities()));
            }
            

            if (it->_f_long_term_debt._dirty){
                fields.push_back(std::string("long_term_debt"));
                row.push_back(toSQLString(it->_long_term_debt()));
            }
            

            if (it->_f_book_value._dirty){
                fields.push_back(std::string("book_value"));
                row.push_back(toSQLString(it->_book_value()));
            }
            

            if (it->_f_created_at._dirty){
                fields.push_back(std::string("created_at"));
                row.push_back(toSQLString(it->_created_at()));
            }
            

            if (it->_f_updated_at._dirty){
                fields.push_back(std::string("updated_at"));
                row.push_back(toSQLString(it->_updated_at()));
            }
            

            if (it->_f_net_tangible_assets._dirty){
                fields.push_back(std::string("net_tangible_assets"));
                row.push_back(toSQLString(it->_net_tangible_assets()));
            }
            

            if (it->_f_quarter._dirty){
                fields.push_back(std::string("quarter"));
                row.push_back(toSQLString(it->_quarter()));
            }
            

            if (it->_f_calculated_bv._dirty){
                fields.push_back(std::string("calculated_bv"));
                row.push_back(toSQLString(it->_calculated_bv()));
            }
            

            if (it->_f_calculated_tl._dirty){
                fields.push_back(std::string("calculated_tl"));
                row.push_back(toSQLString(it->_calculated_tl()));
            }
            fields2Rows[fields].push_back(row);
	}
        bool ret = true;
        for (FieldsToRows::const_iterator it = fields2Rows.begin();
             it != fields2Rows.end(); ++it)
        {
            ret = DBFace::instance()->insert("balance_sheets",
                                             it->first, it->second) && ret;
        }
        return ret;
    }


    Condition                 _constraint;    
    std::vector<std::string>  _tables;
};

} //namespace DMMM
#endif //T_BALANCESHEET
