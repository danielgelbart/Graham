
#ifndef T_OWNEDSTOCK
#define T_OWNEDSTOCK
#include "O_OwnedStock.hpp"

namespace DMMM {

class T_OwnedStock{
public:

    struct Condition{
        bool nil() const { return _cond.size() == 0; }
        std::string _cond;
    };

    T_OwnedStock() 
    {
        _tables.push_back("owned_stocks");
    }
    T_OwnedStock(const Condition& c) 
        : _constraint(c)
    {
        _tables.push_back("owned_stocks");
    }

 
    T_OwnedStock(const I_Stock& parentId)
    {
        _tables.push_back("owned_stocks");
        
        _constraint._cond = "(owned_stocks.stock_id = " + parentId.to_s() + ")";
    } 
 
    T_OwnedStock(const I_Portfolio& parentId)
    {
        _tables.push_back("owned_stocks");
        
        _constraint._cond = "(owned_stocks.portfolio_id = " + parentId.to_s() + ")";
    } 
    
    struct E_id{
        E_id() 
        {  
            _field = "owned_stocks.id";
        }
        std::string _field;
        typedef T_OwnedStock::Condition ConditionType;
        typedef Field<I_OwnedStock>::Base ComparerType;
    };

    static E_id _id(){ 
        return E_id();
    }
    struct E_portfolio_id{
        E_portfolio_id() 
        {  
            _field = "owned_stocks.portfolio_id";
        }
        std::string _field;
        typedef T_OwnedStock::Condition ConditionType;
        typedef F_Fixnum::Base ComparerType;
    };

    static E_portfolio_id _portfolio_id(){ 
        return E_portfolio_id();
    }
    struct E_stock_id{
        E_stock_id() 
        {  
            _field = "owned_stocks.stock_id";
        }
        std::string _field;
        typedef T_OwnedStock::Condition ConditionType;
        typedef F_Fixnum::Base ComparerType;
    };

    static E_stock_id _stock_id(){ 
        return E_stock_id();
    }
    struct E_shares{
        E_shares() 
        {  
            _field = "owned_stocks.shares";
        }
        std::string _field;
        typedef T_OwnedStock::Condition ConditionType;
        typedef F_Fixnum::Base ComparerType;
    };

    static E_shares _shares(){ 
        return E_shares();
    }
    struct E_created_at{
        E_created_at() 
        {  
            _field = "owned_stocks.created_at";
        }
        std::string _field;
        typedef T_OwnedStock::Condition ConditionType;
        typedef F_Time::Base ComparerType;
    };

    static E_created_at _created_at(){ 
        return E_created_at();
    }
    struct E_updated_at{
        E_updated_at() 
        {  
            _field = "owned_stocks.updated_at";
        }
        std::string _field;
        typedef T_OwnedStock::Condition ConditionType;
        typedef F_Time::Base ComparerType;
    };

    static E_updated_at _updated_at(){ 
        return E_updated_at();
    }
    

    std::vector<std::string> getFields()
    {
        std::vector<std::string> ret;
        ret.clear();
        ret.push_back("id");
        ret.push_back("portfolio_id");
        ret.push_back("stock_id");
        ret.push_back("shares");
        ret.push_back("created_at");
        ret.push_back("updated_at");
        return ret;
    }

    std::vector<O_OwnedStock> 
    select(const Condition& c, const std::string& additional)
    {
        std::vector<O_OwnedStock> ret;
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        std::vector<std::string> fields = getFields();
        QueryRes res;
        DBFace::instance()->select(_tables, fields, c1._cond, 
                                   additional, res);
        ret.resize(res.size());
        for(size_t i = 0; i < res.size(); ++i){
            ret[i]._f_id._base =
                UTILS::fromString<Field<I_OwnedStock>::Base>(res[i]["id"]);
            ret[i]._f_portfolio_id._base =
                UTILS::fromString<F_Fixnum::Base>(res[i]["portfolio_id"]);
            ret[i]._f_stock_id._base =
                UTILS::fromString<F_Fixnum::Base>(res[i]["stock_id"]);
            ret[i]._f_shares._base =
                UTILS::fromString<F_Fixnum::Base>(res[i]["shares"]);
            ret[i]._f_created_at._base =
                UTILS::fromString<F_Time::Base>(res[i]["created_at"]);
            ret[i]._f_updated_at._base =
                UTILS::fromString<F_Time::Base>(res[i]["updated_at"]);
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


    std::vector<O_OwnedStock> select(const Condition& c)
    {
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        return select(c1, "");
    }

    std::vector<O_OwnedStock> 
    select(const std::string& additional)
    {
        return select(_constraint, additional);
    }

    std::vector<O_OwnedStock> select()
    {
        return select(_constraint, "");
    }

    std::pair<O_OwnedStock, bool> select(const I_OwnedStock& id){
        return first(E_id() == id);
    }

    std::pair<O_OwnedStock, bool> first(const Condition& c)
    {
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        std::vector<O_OwnedStock> r = select(c, "limit 1");
        if(r.size() > 0)
            return std::make_pair(r[0], true);
        else
            return std::make_pair(O_OwnedStock(), false);
    }

    std::pair<O_OwnedStock, bool> first(){
        return first(_constraint);
    }

    bool insertAllFields(std::vector<O_OwnedStock>::const_iterator begin,
                         std::vector<O_OwnedStock>::const_iterator end)
    {
	std::vector<std::string> fields;
                

        fields.push_back(std::string("portfolio_id"));
        

        fields.push_back(std::string("stock_id"));
        

        fields.push_back(std::string("shares"));
        

        fields.push_back(std::string("created_at"));
        

        fields.push_back(std::string("updated_at"));
	std::vector<std::vector<std::string> > rows;
	for (; begin != end; ++begin){
	    const O_OwnedStock& r = *begin;
	    std::vector<std::string> row;
                        
            row.push_back(toSQLString(r._portfolio_id()));
            
            row.push_back(toSQLString(r._stock_id()));
            
            row.push_back(toSQLString(r._shares()));
            
            row.push_back(toSQLString(r._created_at()));
            
            row.push_back(toSQLString(r._updated_at()));
	    rows.push_back(row);
	}
        return DBFace::instance()->insert("owned_stocks",
					  fields, rows);
    }

    bool insertDirtyFields(std::vector<O_OwnedStock>::const_iterator begin,
                           std::vector<O_OwnedStock>::const_iterator end)
    {
        if (begin == end)
            return true;
        FieldsToRows fields2Rows;
	for (std::vector<O_OwnedStock>::const_iterator it = begin; 
             it != end; ++it)
        {
            std::vector<std::string> fields;
            std::vector<std::string> row;
                        

            if (it->_f_portfolio_id._dirty){
                fields.push_back(std::string("portfolio_id"));
                row.push_back(toSQLString(it->_portfolio_id()));
            }
            

            if (it->_f_stock_id._dirty){
                fields.push_back(std::string("stock_id"));
                row.push_back(toSQLString(it->_stock_id()));
            }
            

            if (it->_f_shares._dirty){
                fields.push_back(std::string("shares"));
                row.push_back(toSQLString(it->_shares()));
            }
            

            if (it->_f_created_at._dirty){
                fields.push_back(std::string("created_at"));
                row.push_back(toSQLString(it->_created_at()));
            }
            

            if (it->_f_updated_at._dirty){
                fields.push_back(std::string("updated_at"));
                row.push_back(toSQLString(it->_updated_at()));
            }
            fields2Rows[fields].push_back(row);
	}
        bool ret = true;
        for (FieldsToRows::const_iterator it = fields2Rows.begin();
             it != fields2Rows.end(); ++it)
        {
            ret = DBFace::instance()->insert("owned_stocks",
                                             it->first, it->second) && ret;
        }
        return ret;
    }


    Condition                 _constraint;    
    std::vector<std::string>  _tables;
};

} //namespace DMMM
#endif //T_OWNEDSTOCK
