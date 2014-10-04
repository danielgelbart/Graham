
#ifndef T_SEARCH
#define T_SEARCH
#include "O_Search.hpp"

namespace DMMM {

class T_Search{
public:

    struct Condition{
        bool nil() const { return _cond.size() == 0; }
        std::string _cond;
    };

    T_Search() 
    {
        _tables.push_back("searches");
    }
    T_Search(const Condition& c) 
        : _constraint(c)
    {
        _tables.push_back("searches");
    }

    
    struct E_id{
        E_id() 
        {  
            _field = "searches.id";
        }
        std::string _field;
        typedef T_Search::Condition ConditionType;
        typedef Field<I_Search>::Base ComparerType;
    };

    static E_id _id(){ 
        return E_id();
    }
    struct E_no_losses{
        E_no_losses() 
        {  
            _field = "searches.no_losses";
        }
        std::string _field;
        typedef T_Search::Condition ConditionType;
        typedef F_Object::Base ComparerType;
    };

    static E_no_losses _no_losses(){ 
        return E_no_losses();
    }
    struct E_divs{
        E_divs() 
        {  
            _field = "searches.divs";
        }
        std::string _field;
        typedef T_Search::Condition ConditionType;
        typedef F_Object::Base ComparerType;
    };

    static E_divs _divs(){ 
        return E_divs();
    }
    struct E_dilution{
        E_dilution() 
        {  
            _field = "searches.dilution";
        }
        std::string _field;
        typedef T_Search::Condition ConditionType;
        typedef F_Float::Base ComparerType;
    };

    static E_dilution _dilution(){ 
        return E_dilution();
    }
    struct E_created_at{
        E_created_at() 
        {  
            _field = "searches.created_at";
        }
        std::string _field;
        typedef T_Search::Condition ConditionType;
        typedef F_Time::Base ComparerType;
    };

    static E_created_at _created_at(){ 
        return E_created_at();
    }
    struct E_updated_at{
        E_updated_at() 
        {  
            _field = "searches.updated_at";
        }
        std::string _field;
        typedef T_Search::Condition ConditionType;
        typedef F_Time::Base ComparerType;
    };

    static E_updated_at _updated_at(){ 
        return E_updated_at();
    }
    struct E_good_balance{
        E_good_balance() 
        {  
            _field = "searches.good_balance";
        }
        std::string _field;
        typedef T_Search::Condition ConditionType;
        typedef F_Object::Base ComparerType;
    };

    static E_good_balance _good_balance(){ 
        return E_good_balance();
    }
    struct E_book{
        E_book() 
        {  
            _field = "searches.book";
        }
        std::string _field;
        typedef T_Search::Condition ConditionType;
        typedef F_Fixnum::Base ComparerType;
    };

    static E_book _book(){ 
        return E_book();
    }
    struct E_current_div{
        E_current_div() 
        {  
            _field = "searches.current_div";
        }
        std::string _field;
        typedef T_Search::Condition ConditionType;
        typedef F_Object::Base ComparerType;
    };

    static E_current_div _current_div(){ 
        return E_current_div();
    }
    struct E_earning_growth{
        E_earning_growth() 
        {  
            _field = "searches.earning_growth";
        }
        std::string _field;
        typedef T_Search::Condition ConditionType;
        typedef F_Object::Base ComparerType;
    };

    static E_earning_growth _earning_growth(){ 
        return E_earning_growth();
    }
    struct E_defensive_price{
        E_defensive_price() 
        {  
            _field = "searches.defensive_price";
        }
        std::string _field;
        typedef T_Search::Condition ConditionType;
        typedef F_Object::Base ComparerType;
    };

    static E_defensive_price _defensive_price(){ 
        return E_defensive_price();
    }
    struct E_big_enough{
        E_big_enough() 
        {  
            _field = "searches.big_enough";
        }
        std::string _field;
        typedef T_Search::Condition ConditionType;
        typedef F_Object::Base ComparerType;
    };

    static E_big_enough _big_enough(){ 
        return E_big_enough();
    }
    struct E_market_cap{
        E_market_cap() 
        {  
            _field = "searches.market_cap";
        }
        std::string _field;
        typedef T_Search::Condition ConditionType;
        typedef F_Fixnum::Base ComparerType;
    };

    static E_market_cap _market_cap(){ 
        return E_market_cap();
    }
    

    std::vector<std::string> getFields()
    {
        std::vector<std::string> ret;
        ret.clear();
        ret.push_back("id");
        ret.push_back("no_losses");
        ret.push_back("divs");
        ret.push_back("dilution");
        ret.push_back("created_at");
        ret.push_back("updated_at");
        ret.push_back("good_balance");
        ret.push_back("book");
        ret.push_back("current_div");
        ret.push_back("earning_growth");
        ret.push_back("defensive_price");
        ret.push_back("big_enough");
        ret.push_back("market_cap");
        return ret;
    }

    std::vector<O_Search> 
    select(const Condition& c, const std::string& additional)
    {
        std::vector<O_Search> ret;
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        std::vector<std::string> fields = getFields();
        QueryRes res;
        DBFace::instance()->select(_tables, fields, c1._cond, 
                                   additional, res);
        ret.resize(res.size());
        for(size_t i = 0; i < res.size(); ++i){
            ret[i]._f_id._base =
                UTILS::fromString<Field<I_Search>::Base>(res[i]["id"]);
            ret[i]._f_no_losses._base =
                UTILS::fromString<F_Object::Base>(res[i]["no_losses"]);
            ret[i]._f_divs._base =
                UTILS::fromString<F_Object::Base>(res[i]["divs"]);
            ret[i]._f_dilution._base =
                UTILS::fromString<F_Float::Base>(res[i]["dilution"]);
            ret[i]._f_created_at._base =
                UTILS::fromString<F_Time::Base>(res[i]["created_at"]);
            ret[i]._f_updated_at._base =
                UTILS::fromString<F_Time::Base>(res[i]["updated_at"]);
            ret[i]._f_good_balance._base =
                UTILS::fromString<F_Object::Base>(res[i]["good_balance"]);
            ret[i]._f_book._base =
                UTILS::fromString<F_Fixnum::Base>(res[i]["book"]);
            ret[i]._f_current_div._base =
                UTILS::fromString<F_Object::Base>(res[i]["current_div"]);
            ret[i]._f_earning_growth._base =
                UTILS::fromString<F_Object::Base>(res[i]["earning_growth"]);
            ret[i]._f_defensive_price._base =
                UTILS::fromString<F_Object::Base>(res[i]["defensive_price"]);
            ret[i]._f_big_enough._base =
                UTILS::fromString<F_Object::Base>(res[i]["big_enough"]);
            ret[i]._f_market_cap._base =
                UTILS::fromString<F_Fixnum::Base>(res[i]["market_cap"]);
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


    std::vector<O_Search> select(const Condition& c)
    {
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        return select(c1, "");
    }

    std::vector<O_Search> 
    select(const std::string& additional)
    {
        return select(_constraint, additional);
    }

    std::vector<O_Search> select()
    {
        return select(_constraint, "");
    }

    std::pair<O_Search, bool> select(const I_Search& id){
        return first(E_id() == id);
    }

    std::pair<O_Search, bool> first(const Condition& c)
    {
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        std::vector<O_Search> r = select(c, "limit 1");
        if(r.size() > 0)
            return std::make_pair(r[0], true);
        else
            return std::make_pair(O_Search(), false);
    }

    std::pair<O_Search, bool> first(){
        return first(_constraint);
    }

    bool insertAllFields(std::vector<O_Search>::const_iterator begin,
                         std::vector<O_Search>::const_iterator end)
    {
	std::vector<std::string> fields;
                

        fields.push_back(std::string("no_losses"));
        

        fields.push_back(std::string("divs"));
        

        fields.push_back(std::string("dilution"));
        

        fields.push_back(std::string("created_at"));
        

        fields.push_back(std::string("updated_at"));
        

        fields.push_back(std::string("good_balance"));
        

        fields.push_back(std::string("book"));
        

        fields.push_back(std::string("current_div"));
        

        fields.push_back(std::string("earning_growth"));
        

        fields.push_back(std::string("defensive_price"));
        

        fields.push_back(std::string("big_enough"));
        

        fields.push_back(std::string("market_cap"));
	std::vector<std::vector<std::string> > rows;
	for (; begin != end; ++begin){
	    const O_Search& r = *begin;
	    std::vector<std::string> row;
                        
            row.push_back(toSQLString(r._no_losses()));
            
            row.push_back(toSQLString(r._divs()));
            
            row.push_back(toSQLString(r._dilution()));
            
            row.push_back(toSQLString(r._created_at()));
            
            row.push_back(toSQLString(r._updated_at()));
            
            row.push_back(toSQLString(r._good_balance()));
            
            row.push_back(toSQLString(r._book()));
            
            row.push_back(toSQLString(r._current_div()));
            
            row.push_back(toSQLString(r._earning_growth()));
            
            row.push_back(toSQLString(r._defensive_price()));
            
            row.push_back(toSQLString(r._big_enough()));
            
            row.push_back(toSQLString(r._market_cap()));
	    rows.push_back(row);
	}
        return DBFace::instance()->insert("searches",
					  fields, rows);
    }

    bool insertDirtyFields(std::vector<O_Search>::const_iterator begin,
                           std::vector<O_Search>::const_iterator end)
    {
        if (begin == end)
            return true;
        FieldsToRows fields2Rows;
	for (std::vector<O_Search>::const_iterator it = begin; 
             it != end; ++it)
        {
            std::vector<std::string> fields;
            std::vector<std::string> row;
                        

            if (it->_f_no_losses._dirty){
                fields.push_back(std::string("no_losses"));
                row.push_back(toSQLString(it->_no_losses()));
            }
            

            if (it->_f_divs._dirty){
                fields.push_back(std::string("divs"));
                row.push_back(toSQLString(it->_divs()));
            }
            

            if (it->_f_dilution._dirty){
                fields.push_back(std::string("dilution"));
                row.push_back(toSQLString(it->_dilution()));
            }
            

            if (it->_f_created_at._dirty){
                fields.push_back(std::string("created_at"));
                row.push_back(toSQLString(it->_created_at()));
            }
            

            if (it->_f_updated_at._dirty){
                fields.push_back(std::string("updated_at"));
                row.push_back(toSQLString(it->_updated_at()));
            }
            

            if (it->_f_good_balance._dirty){
                fields.push_back(std::string("good_balance"));
                row.push_back(toSQLString(it->_good_balance()));
            }
            

            if (it->_f_book._dirty){
                fields.push_back(std::string("book"));
                row.push_back(toSQLString(it->_book()));
            }
            

            if (it->_f_current_div._dirty){
                fields.push_back(std::string("current_div"));
                row.push_back(toSQLString(it->_current_div()));
            }
            

            if (it->_f_earning_growth._dirty){
                fields.push_back(std::string("earning_growth"));
                row.push_back(toSQLString(it->_earning_growth()));
            }
            

            if (it->_f_defensive_price._dirty){
                fields.push_back(std::string("defensive_price"));
                row.push_back(toSQLString(it->_defensive_price()));
            }
            

            if (it->_f_big_enough._dirty){
                fields.push_back(std::string("big_enough"));
                row.push_back(toSQLString(it->_big_enough()));
            }
            

            if (it->_f_market_cap._dirty){
                fields.push_back(std::string("market_cap"));
                row.push_back(toSQLString(it->_market_cap()));
            }
            fields2Rows[fields].push_back(row);
	}
        bool ret = true;
        for (FieldsToRows::const_iterator it = fields2Rows.begin();
             it != fields2Rows.end(); ++it)
        {
            ret = DBFace::instance()->insert("searches",
                                             it->first, it->second) && ret;
        }
        return ret;
    }


    Condition                 _constraint;    
    std::vector<std::string>  _tables;
};

} //namespace DMMM
#endif //T_SEARCH
