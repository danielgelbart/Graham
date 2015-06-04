
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
    struct E_base{
        E_base() 
        {  
            _field = "splits.base";
        }
        std::string _field;
        typedef T_Split::Condition ConditionType;
        typedef F_Fixnum::Base ComparerType;
    };

    static E_base _base(){ 
        return E_base();
    }
    struct E_into{
        E_into() 
        {  
            _field = "splits.into";
        }
        std::string _field;
        typedef T_Split::Condition ConditionType;
        typedef F_Fixnum::Base ComparerType;
    };

    static E_into _into(){ 
        return E_into();
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
        ret.push_back("date");
        ret.push_back("base");
        ret.push_back("into");
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
            ret[i]._f_date._base =
                UTILS::fromString<F_Date::Base>(res[i]["date"]);
            ret[i]._f_base._base =
                UTILS::fromString<F_Fixnum::Base>(res[i]["base"]);
            ret[i]._f_into._base =
                UTILS::fromString<F_Fixnum::Base>(res[i]["into"]);
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
        

        fields.push_back(std::string("date"));
        

        fields.push_back(std::string("base"));
        

        fields.push_back(std::string("into"));
        

        fields.push_back(std::string("created_at"));
        

        fields.push_back(std::string("updated_at"));
	std::vector<std::vector<std::string> > rows;
	for (; begin != end; ++begin){
	    const O_Split& r = *begin;
	    std::vector<std::string> row;
                        
            row.push_back(toSQLString(r._stock_id()));
            
            row.push_back(toSQLString(r._date()));
            
            row.push_back(toSQLString(r._base()));
            
            row.push_back(toSQLString(r._into()));
            
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
            

            if (it->_f_date._dirty){
                fields.push_back(std::string("date"));
                row.push_back(toSQLString(it->_date()));
            }
            

            if (it->_f_base._dirty){
                fields.push_back(std::string("base"));
                row.push_back(toSQLString(it->_base()));
            }
            

            if (it->_f_into._dirty){
                fields.push_back(std::string("into"));
                row.push_back(toSQLString(it->_into()));
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
