
#ifndef T_SPLIT
#define T_SPLIT
#include "O_Split.hpp"

namespace DMMM {

class T_Split{
public:

    struct Condition{
        bool nil() const { return _cond.size() == 0; }
        std::string _cond;
    };

    T_Split() 
    {
        _tables.push_back("splits");
    }
    T_Split(const Condition& c) 
        : _constraint(c)
    {
        _tables.push_back("splits");
    }

 
    T_Split(const I_Stock& parentId)
    {
        _tables.push_back("splits");
        
        _constraint._cond = "(splits.stock_id = " + parentId.to_s() + ")";
    } 
    
    struct E_id{
        E_id() 
        {  
            _field = "splits.id";
        }
        std::string _field;
        typedef T_Split::Condition ConditionType;
        typedef Field<I_Split>::Base ComparerType;
    };

    static E_id _id(){ 
        return E_id();
    }
    struct E_stock_id{
        E_stock_id() 
        {  
            _field = "splits.stock_id";
        }
        std::string _field;
        typedef T_Split::Condition ConditionType;
        typedef F_Fixnum::Base ComparerType;
    };

    static E_stock_id _stock_id(){ 
        return E_stock_id();
    }
    struct E_old{
        E_old() 
        {  
            _field = "splits.old";
        }
        std::string _field;
        typedef T_Split::Condition ConditionType;
        typedef F_Fixnum::Base ComparerType;
    };

    static E_old _old(){ 
        return E_old();
    }
    struct E_new{
        E_new() 
        {  
            _field = "splits.new";
        }
        std::string _field;
        typedef T_Split::Condition ConditionType;
        typedef F_Fixnum::Base ComparerType;
    };

    static E_new _new(){ 
        return E_new();
    }
    struct E_split_mul_factor{
        E_split_mul_factor() 
        {  
            _field = "splits.split_mul_factor";
        }
        std::string _field;
        typedef T_Split::Condition ConditionType;
        typedef F_BigDecimal::Base ComparerType;
    };

    static E_split_mul_factor _split_mul_factor(){ 
        return E_split_mul_factor();
    }
    struct E_date{
        E_date() 
        {  
            _field = "splits.date";
        }
        std::string _field;
        typedef T_Split::Condition ConditionType;
        typedef F_Date::Base ComparerType;
    };

    static E_date _date(){ 
        return E_date();
    }
    struct E_stock_adjusted_for_split{
        E_stock_adjusted_for_split() 
        {  
            _field = "splits.stock_adjusted_for_split";
        }
        std::string _field;
        typedef T_Split::Condition ConditionType;
        typedef F_Object::Base ComparerType;
    };

    static E_stock_adjusted_for_split _stock_adjusted_for_split(){ 
        return E_stock_adjusted_for_split();
    }
    struct E_created_at{
        E_created_at() 
        {  
            _field = "splits.created_at";
        }
        std::string _field;
        typedef T_Split::Condition ConditionType;
        typedef F_Time::Base ComparerType;
    };

    static E_created_at _created_at(){ 
        return E_created_at();
    }
    struct E_updated_at{
        E_updated_at() 
        {  
            _field = "splits.updated_at";
        }
        std::string _field;
        typedef T_Split::Condition ConditionType;
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
        ret.push_back("stock_id");
        ret.push_back("old");
        ret.push_back("new");
        ret.push_back("split_mul_factor");
        ret.push_back("date");
        ret.push_back("stock_adjusted_for_split");
        ret.push_back("created_at");
        ret.push_back("updated_at");
        return ret;
    }

    std::vector<O_Split> 
    select(const Condition& c, const std::string& additional)
    {
        std::vector<O_Split> ret;
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        std::vector<std::string> fields = getFields();
        QueryRes res;
        DBFace::instance()->select(_tables, fields, c1._cond, 
                                   additional, res);
        ret.resize(res.size());
        for(size_t i = 0; i < res.size(); ++i){
            ret[i]._f_id._base =
                UTILS::fromString<Field<I_Split>::Base>(res[i]["id"]);
            ret[i]._f_stock_id._base =
                UTILS::fromString<F_Fixnum::Base>(res[i]["stock_id"]);
            ret[i]._f_old._base =
                UTILS::fromString<F_Fixnum::Base>(res[i]["old"]);
            ret[i]._f_new._base =
                UTILS::fromString<F_Fixnum::Base>(res[i]["new"]);
            ret[i]._f_split_mul_factor._base =
                UTILS::fromString<F_BigDecimal::Base>(res[i]["split_mul_factor"]);
            ret[i]._f_date._base =
                UTILS::fromString<F_Date::Base>(res[i]["date"]);
            ret[i]._f_stock_adjusted_for_split._base =
                UTILS::fromString<F_Object::Base>(res[i]["stock_adjusted_for_split"]);
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


    std::vector<O_Split> select(const Condition& c)
    {
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        return select(c1, "");
    }

    std::vector<O_Split> 
    select(const std::string& additional)
    {
        return select(_constraint, additional);
    }

    std::vector<O_Split> select()
    {
        return select(_constraint, "");
    }

    std::pair<O_Split, bool> select(const I_Split& id){
        return first(E_id() == id);
    }

    std::pair<O_Split, bool> first(const Condition& c)
    {
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        std::vector<O_Split> r = select(c, "limit 1");
        if(r.size() > 0)
            return std::make_pair(r[0], true);
        else
            return std::make_pair(O_Split(), false);
    }

    std::pair<O_Split, bool> first(){
        return first(_constraint);
    }

    bool insertAllFields(std::vector<O_Split>::const_iterator begin,
                         std::vector<O_Split>::const_iterator end)
    {
	std::vector<std::string> fields;
                

        fields.push_back(std::string("stock_id"));
        

        fields.push_back(std::string("old"));
        

        fields.push_back(std::string("new"));
        

        fields.push_back(std::string("split_mul_factor"));
        

        fields.push_back(std::string("date"));
        

        fields.push_back(std::string("stock_adjusted_for_split"));
        

        fields.push_back(std::string("created_at"));
        

        fields.push_back(std::string("updated_at"));
	std::vector<std::vector<std::string> > rows;
	for (; begin != end; ++begin){
	    const O_Split& r = *begin;
	    std::vector<std::string> row;
                        
            row.push_back(toSQLString(r._stock_id()));
            
            row.push_back(toSQLString(r._old()));
            
            row.push_back(toSQLString(r._new()));
            
            row.push_back(toSQLString(r._split_mul_factor()));
            
            row.push_back(toSQLString(r._date()));
            
            row.push_back(toSQLString(r._stock_adjusted_for_split()));
            
            row.push_back(toSQLString(r._created_at()));
            
            row.push_back(toSQLString(r._updated_at()));
	    rows.push_back(row);
	}
        return DBFace::instance()->insert("splits",
					  fields, rows);
    }

    bool insertDirtyFields(std::vector<O_Split>::const_iterator begin,
                           std::vector<O_Split>::const_iterator end)
    {
        if (begin == end)
            return true;
        FieldsToRows fields2Rows;
	for (std::vector<O_Split>::const_iterator it = begin; 
             it != end; ++it)
        {
            std::vector<std::string> fields;
            std::vector<std::string> row;
                        

            if (it->_f_stock_id._dirty){
                fields.push_back(std::string("stock_id"));
                row.push_back(toSQLString(it->_stock_id()));
            }
            

            if (it->_f_old._dirty){
                fields.push_back(std::string("old"));
                row.push_back(toSQLString(it->_old()));
            }
            

            if (it->_f_new._dirty){
                fields.push_back(std::string("new"));
                row.push_back(toSQLString(it->_new()));
            }
            

            if (it->_f_split_mul_factor._dirty){
                fields.push_back(std::string("split_mul_factor"));
                row.push_back(toSQLString(it->_split_mul_factor()));
            }
            

            if (it->_f_date._dirty){
                fields.push_back(std::string("date"));
                row.push_back(toSQLString(it->_date()));
            }
            

            if (it->_f_stock_adjusted_for_split._dirty){
                fields.push_back(std::string("stock_adjusted_for_split"));
                row.push_back(toSQLString(it->_stock_adjusted_for_split()));
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
            ret = DBFace::instance()->insert("splits",
                                             it->first, it->second) && ret;
        }
        return ret;
    }


    Condition                 _constraint;    
    std::vector<std::string>  _tables;
};

} //namespace DMMM
#endif //T_SPLIT
