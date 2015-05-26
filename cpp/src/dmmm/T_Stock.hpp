
#ifndef T_STOCK
#define T_STOCK
#include "O_Stock.hpp"

namespace DMMM {

class T_Stock{
public:

    struct Condition{
        bool nil() const { return _cond.size() == 0; }
        std::string _cond;
    };

    T_Stock() 
    {
        _tables.push_back("stocks");
    }
    T_Stock(const Condition& c) 
        : _constraint(c)
    {
        _tables.push_back("stocks");
    }

    
    struct E_id{
        E_id() 
        {  
            _field = "stocks.id";
        }
        std::string _field;
        typedef T_Stock::Condition ConditionType;
        typedef Field<I_Stock>::Base ComparerType;
    };

    static E_id _id(){ 
        return E_id();
    }
    struct E_name{
        E_name() 
        {  
            _field = "stocks.name";
        }
        std::string _field;
        typedef T_Stock::Condition ConditionType;
        typedef F_String::Base ComparerType;
    };

    static E_name _name(){ 
        return E_name();
    }
    struct E_ticker{
        E_ticker() 
        {  
            _field = "stocks.ticker";
        }
        std::string _field;
        typedef T_Stock::Condition ConditionType;
        typedef F_String::Base ComparerType;
    };

    static E_ticker _ticker(){ 
        return E_ticker();
    }
    struct E_created_at{
        E_created_at() 
        {  
            _field = "stocks.created_at";
        }
        std::string _field;
        typedef T_Stock::Condition ConditionType;
        typedef F_Time::Base ComparerType;
    };

    static E_created_at _created_at(){ 
        return E_created_at();
    }
    struct E_updated_at{
        E_updated_at() 
        {  
            _field = "stocks.updated_at";
        }
        std::string _field;
        typedef T_Stock::Condition ConditionType;
        typedef F_Time::Base ComparerType;
    };

    static E_updated_at _updated_at(){ 
        return E_updated_at();
    }
    struct E_dividends_per_year{
        E_dividends_per_year() 
        {  
            _field = "stocks.dividends_per_year";
        }
        std::string _field;
        typedef T_Stock::Condition ConditionType;
        typedef F_Fixnum::Base ComparerType;
    };

    static E_dividends_per_year _dividends_per_year(){ 
        return E_dividends_per_year();
    }
    struct E_latest_price{
        E_latest_price() 
        {  
            _field = "stocks.latest_price";
        }
        std::string _field;
        typedef T_Stock::Condition ConditionType;
        typedef F_BigDecimal::Base ComparerType;
    };

    static E_latest_price _latest_price(){ 
        return E_latest_price();
    }
    struct E_market_cap{
        E_market_cap() 
        {  
            _field = "stocks.market_cap";
        }
        std::string _field;
        typedef T_Stock::Condition ConditionType;
        typedef F_String::Base ComparerType;
    };

    static E_market_cap _market_cap(){ 
        return E_market_cap();
    }
    struct E_ttm_div{
        E_ttm_div() 
        {  
            _field = "stocks.ttm_div";
        }
        std::string _field;
        typedef T_Stock::Condition ConditionType;
        typedef F_BigDecimal::Base ComparerType;
    };

    static E_ttm_div _ttm_div(){ 
        return E_ttm_div();
    }
    struct E_yield{
        E_yield() 
        {  
            _field = "stocks.yield";
        }
        std::string _field;
        typedef T_Stock::Condition ConditionType;
        typedef F_BigDecimal::Base ComparerType;
    };

    static E_yield _yield(){ 
        return E_yield();
    }
    struct E_listed{
        E_listed() 
        {  
            _field = "stocks.listed";
        }
        std::string _field;
        typedef T_Stock::Condition ConditionType;
        typedef F_Object::Base ComparerType;
    };

    static E_listed _listed(){ 
        return E_listed();
    }
    struct E_has_currant_ratio{
        E_has_currant_ratio() 
        {  
            _field = "stocks.has_currant_ratio";
        }
        std::string _field;
        typedef T_Stock::Condition ConditionType;
        typedef F_Object::Base ComparerType;
    };

    static E_has_currant_ratio _has_currant_ratio(){ 
        return E_has_currant_ratio();
    }
    struct E_mark{
        E_mark() 
        {  
            _field = "stocks.mark";
        }
        std::string _field;
        typedef T_Stock::Condition ConditionType;
        typedef F_String::Base ComparerType;
    };

    static E_mark _mark(){ 
        return E_mark();
    }
    struct E_cik{
        E_cik() 
        {  
            _field = "stocks.cik";
        }
        std::string _field;
        typedef T_Stock::Condition ConditionType;
        typedef F_Fixnum::Base ComparerType;
    };

    static E_cik _cik(){ 
        return E_cik();
    }
    struct E_fiscal_year_end{
        E_fiscal_year_end() 
        {  
            _field = "stocks.fiscal_year_end";
        }
        std::string _field;
        typedef T_Stock::Condition ConditionType;
        typedef F_String::Base ComparerType;
    };

    static E_fiscal_year_end _fiscal_year_end(){ 
        return E_fiscal_year_end();
    }
    struct E_company_type{
        E_company_type() 
        {  
            _field = "stocks.company_type";
        }
        std::string _field;
        typedef T_Stock::Condition ConditionType;
        typedef Field<EnumStockCOMPANY_TYPE>::Base ComparerType;
    };

    static E_company_type _company_type(){ 
        return E_company_type();
    }
    struct E_country{
        E_country() 
        {  
            _field = "stocks.country";
        }
        std::string _field;
        typedef T_Stock::Condition ConditionType;
        typedef F_String::Base ComparerType;
    };

    static E_country _country(){ 
        return E_country();
    }
    struct E_fy_same_as_ed{
        E_fy_same_as_ed() 
        {  
            _field = "stocks.fy_same_as_ed";
        }
        std::string _field;
        typedef T_Stock::Condition ConditionType;
        typedef F_Object::Base ComparerType;
    };

    static E_fy_same_as_ed _fy_same_as_ed(){ 
        return E_fy_same_as_ed();
    }
    

    std::vector<std::string> getFields()
    {
        std::vector<std::string> ret;
        ret.clear();
        ret.push_back("id");
        ret.push_back("name");
        ret.push_back("ticker");
        ret.push_back("created_at");
        ret.push_back("updated_at");
        ret.push_back("dividends_per_year");
        ret.push_back("latest_price");
        ret.push_back("market_cap");
        ret.push_back("ttm_div");
        ret.push_back("yield");
        ret.push_back("listed");
        ret.push_back("has_currant_ratio");
        ret.push_back("mark");
        ret.push_back("cik");
        ret.push_back("fiscal_year_end");
        ret.push_back("company_type+0");
        ret.push_back("country");
        ret.push_back("fy_same_as_ed");
        return ret;
    }

    std::vector<O_Stock> 
    select(const Condition& c, const std::string& additional)
    {
        std::vector<O_Stock> ret;
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        std::vector<std::string> fields = getFields();
        QueryRes res;
        DBFace::instance()->select(_tables, fields, c1._cond, 
                                   additional, res);
        ret.resize(res.size());
        for(size_t i = 0; i < res.size(); ++i){
            ret[i]._f_id._base =
                UTILS::fromString<Field<I_Stock>::Base>(res[i]["id"]);
            ret[i]._f_name._base =
                UTILS::fromString<F_String::Base>(res[i]["name"]);
            ret[i]._f_ticker._base =
                UTILS::fromString<F_String::Base>(res[i]["ticker"]);
            ret[i]._f_created_at._base =
                UTILS::fromString<F_Time::Base>(res[i]["created_at"]);
            ret[i]._f_updated_at._base =
                UTILS::fromString<F_Time::Base>(res[i]["updated_at"]);
            ret[i]._f_dividends_per_year._base =
                UTILS::fromString<F_Fixnum::Base>(res[i]["dividends_per_year"]);
            ret[i]._f_latest_price._base =
                UTILS::fromString<F_BigDecimal::Base>(res[i]["latest_price"]);
            ret[i]._f_market_cap._base =
                UTILS::fromString<F_String::Base>(res[i]["market_cap"]);
            ret[i]._f_ttm_div._base =
                UTILS::fromString<F_BigDecimal::Base>(res[i]["ttm_div"]);
            ret[i]._f_yield._base =
                UTILS::fromString<F_BigDecimal::Base>(res[i]["yield"]);
            ret[i]._f_listed._base =
                UTILS::fromString<F_Object::Base>(res[i]["listed"]);
            ret[i]._f_has_currant_ratio._base =
                UTILS::fromString<F_Object::Base>(res[i]["has_currant_ratio"]);
            ret[i]._f_mark._base =
                UTILS::fromString<F_String::Base>(res[i]["mark"]);
            ret[i]._f_cik._base =
                UTILS::fromString<F_Fixnum::Base>(res[i]["cik"]);
            ret[i]._f_fiscal_year_end._base =
                UTILS::fromString<F_String::Base>(res[i]["fiscal_year_end"]);
            ret[i]._f_company_type._base =
                (Field<EnumStockCOMPANY_TYPE>::Base)UTILS::fromString<size_t>(res[i]["company_type+0"]);
            ret[i]._f_country._base =
                UTILS::fromString<F_String::Base>(res[i]["country"]);
            ret[i]._f_fy_same_as_ed._base =
                UTILS::fromString<F_Object::Base>(res[i]["fy_same_as_ed"]);
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


    std::vector<O_Stock> select(const Condition& c)
    {
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        return select(c1, "");
    }

    std::vector<O_Stock> 
    select(const std::string& additional)
    {
        return select(_constraint, additional);
    }

    std::vector<O_Stock> select()
    {
        return select(_constraint, "");
    }

    std::pair<O_Stock, bool> select(const I_Stock& id){
        return first(E_id() == id);
    }

    std::pair<O_Stock, bool> first(const Condition& c)
    {
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        std::vector<O_Stock> r = select(c, "limit 1");
        if(r.size() > 0)
            return std::make_pair(r[0], true);
        else
            return std::make_pair(O_Stock(), false);
    }

    std::pair<O_Stock, bool> first(){
        return first(_constraint);
    }

    bool insertAllFields(std::vector<O_Stock>::const_iterator begin,
                         std::vector<O_Stock>::const_iterator end)
    {
	std::vector<std::string> fields;
                

        fields.push_back(std::string("name"));
        

        fields.push_back(std::string("ticker"));
        

        fields.push_back(std::string("created_at"));
        

        fields.push_back(std::string("updated_at"));
        

        fields.push_back(std::string("dividends_per_year"));
        

        fields.push_back(std::string("latest_price"));
        

        fields.push_back(std::string("market_cap"));
        

        fields.push_back(std::string("ttm_div"));
        

        fields.push_back(std::string("yield"));
        

        fields.push_back(std::string("listed"));
        

        fields.push_back(std::string("has_currant_ratio"));
        

        fields.push_back(std::string("mark"));
        

        fields.push_back(std::string("cik"));
        

        fields.push_back(std::string("fiscal_year_end"));
        

        fields.push_back(std::string("company_type"));
        

        fields.push_back(std::string("country"));
        

        fields.push_back(std::string("fy_same_as_ed"));
	std::vector<std::vector<std::string> > rows;
	for (; begin != end; ++begin){
	    const O_Stock& r = *begin;
	    std::vector<std::string> row;
                        
            row.push_back(toSQLString(r._name()));
            
            row.push_back(toSQLString(r._ticker()));
            
            row.push_back(toSQLString(r._created_at()));
            
            row.push_back(toSQLString(r._updated_at()));
            
            row.push_back(toSQLString(r._dividends_per_year()));
            
            row.push_back(toSQLString(r._latest_price()));
            
            row.push_back(toSQLString(r._market_cap()));
            
            row.push_back(toSQLString(r._ttm_div()));
            
            row.push_back(toSQLString(r._yield()));
            
            row.push_back(toSQLString(r._listed()));
            
            row.push_back(toSQLString(r._has_currant_ratio()));
            
            row.push_back(toSQLString(r._mark()));
            
            row.push_back(toSQLString(r._cik()));
            
            row.push_back(toSQLString(r._fiscal_year_end()));
            
            row.push_back(toSQLString(r._company_type()));
            
            row.push_back(toSQLString(r._country()));
            
            row.push_back(toSQLString(r._fy_same_as_ed()));
	    rows.push_back(row);
	}
        return DBFace::instance()->insert("stocks",
					  fields, rows);
    }

    bool insertDirtyFields(std::vector<O_Stock>::const_iterator begin,
                           std::vector<O_Stock>::const_iterator end)
    {
        if (begin == end)
            return true;
        FieldsToRows fields2Rows;
	for (std::vector<O_Stock>::const_iterator it = begin; 
             it != end; ++it)
        {
            std::vector<std::string> fields;
            std::vector<std::string> row;
                        

            if (it->_f_name._dirty){
                fields.push_back(std::string("name"));
                row.push_back(toSQLString(it->_name()));
            }
            

            if (it->_f_ticker._dirty){
                fields.push_back(std::string("ticker"));
                row.push_back(toSQLString(it->_ticker()));
            }
            

            if (it->_f_created_at._dirty){
                fields.push_back(std::string("created_at"));
                row.push_back(toSQLString(it->_created_at()));
            }
            

            if (it->_f_updated_at._dirty){
                fields.push_back(std::string("updated_at"));
                row.push_back(toSQLString(it->_updated_at()));
            }
            

            if (it->_f_dividends_per_year._dirty){
                fields.push_back(std::string("dividends_per_year"));
                row.push_back(toSQLString(it->_dividends_per_year()));
            }
            

            if (it->_f_latest_price._dirty){
                fields.push_back(std::string("latest_price"));
                row.push_back(toSQLString(it->_latest_price()));
            }
            

            if (it->_f_market_cap._dirty){
                fields.push_back(std::string("market_cap"));
                row.push_back(toSQLString(it->_market_cap()));
            }
            

            if (it->_f_ttm_div._dirty){
                fields.push_back(std::string("ttm_div"));
                row.push_back(toSQLString(it->_ttm_div()));
            }
            

            if (it->_f_yield._dirty){
                fields.push_back(std::string("yield"));
                row.push_back(toSQLString(it->_yield()));
            }
            

            if (it->_f_listed._dirty){
                fields.push_back(std::string("listed"));
                row.push_back(toSQLString(it->_listed()));
            }
            

            if (it->_f_has_currant_ratio._dirty){
                fields.push_back(std::string("has_currant_ratio"));
                row.push_back(toSQLString(it->_has_currant_ratio()));
            }
            

            if (it->_f_mark._dirty){
                fields.push_back(std::string("mark"));
                row.push_back(toSQLString(it->_mark()));
            }
            

            if (it->_f_cik._dirty){
                fields.push_back(std::string("cik"));
                row.push_back(toSQLString(it->_cik()));
            }
            

            if (it->_f_fiscal_year_end._dirty){
                fields.push_back(std::string("fiscal_year_end"));
                row.push_back(toSQLString(it->_fiscal_year_end()));
            }
            

            if (it->_f_company_type._dirty){
                fields.push_back(std::string("company_type"));
                row.push_back(toSQLString(it->_company_type()));
            }
            

            if (it->_f_country._dirty){
                fields.push_back(std::string("country"));
                row.push_back(toSQLString(it->_country()));
            }
            

            if (it->_f_fy_same_as_ed._dirty){
                fields.push_back(std::string("fy_same_as_ed"));
                row.push_back(toSQLString(it->_fy_same_as_ed()));
            }
            fields2Rows[fields].push_back(row);
	}
        bool ret = true;
        for (FieldsToRows::const_iterator it = fields2Rows.begin();
             it != fields2Rows.end(); ++it)
        {
            ret = DBFace::instance()->insert("stocks",
                                             it->first, it->second) && ret;
        }
        return ret;
    }


    Condition                 _constraint;    
    std::vector<std::string>  _tables;
};

} //namespace DMMM
#endif //T_STOCK
