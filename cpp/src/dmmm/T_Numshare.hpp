
#ifndef T_NUMSHARE
#define T_NUMSHARE
#include "O_Numshare.hpp"

namespace DMMM {

class T_Numshare{
public:

    struct Condition{
        bool nil() const { return _cond.size() == 0; }
        std::string _cond;
    };

    T_Numshare() 
    {
        _tables.push_back("numshares");
    }
    T_Numshare(const Condition& c) 
        : _constraint(c)
    {
        _tables.push_back("numshares");
    }

 
    T_Numshare(const I_Stock& parentId)
    {
        _tables.push_back("numshares");
        
        _constraint._cond = "(numshares.stock_id = " + parentId.to_s() + ")";
    } 
    
    struct E_id{
        E_id() 
        {  
            _field = "numshares.id";
        }
        std::string _field;
        typedef T_Numshare::Condition ConditionType;
        typedef Field<I_Numshare>::Base ComparerType;
    };

    static E_id _id(){ 
        return E_id();
    }
    struct E_stock_id{
        E_stock_id() 
        {  
            _field = "numshares.stock_id";
        }
        std::string _field;
        typedef T_Numshare::Condition ConditionType;
        typedef F_Fixnum::Base ComparerType;
    };

    static E_stock_id _stock_id(){ 
        return E_stock_id();
    }
    struct E_year{
        E_year() 
        {  
            _field = "numshares.year";
        }
        std::string _field;
        typedef T_Numshare::Condition ConditionType;
        typedef F_Fixnum::Base ComparerType;
    };

    static E_year _year(){ 
        return E_year();
    }
    struct E_shares{
        E_shares() 
        {  
            _field = "numshares.shares";
        }
        std::string _field;
        typedef T_Numshare::Condition ConditionType;
        typedef F_String::Base ComparerType;
    };

    static E_shares _shares(){ 
        return E_shares();
    }
    struct E_created_at{
        E_created_at() 
        {  
            _field = "numshares.created_at";
        }
        std::string _field;
        typedef T_Numshare::Condition ConditionType;
        typedef F_Time::Base ComparerType;
    };

    static E_created_at _created_at(){ 
        return E_created_at();
    }
    struct E_updated_at{
        E_updated_at() 
        {  
            _field = "numshares.updated_at";
        }
        std::string _field;
        typedef T_Numshare::Condition ConditionType;
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
        ret.push_back("year");
        ret.push_back("shares");
        ret.push_back("created_at");
        ret.push_back("updated_at");
        return ret;
    }

    std::vector<O_Numshare> 
    select(const Condition& c, const std::string& additional)
    {
        std::vector<O_Numshare> ret;
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        std::vector<std::string> fields = getFields();
        QueryRes res;
        DBFace::instance()->select(_tables, fields, c1._cond, 
                                   additional, res);
        ret.resize(res.size());
        for(size_t i = 0; i < res.size(); ++i){
            ret[i]._f_id._base =
                UTILS::fromString<Field<I_Numshare>::Base>(res[i]["id"]);
            ret[i]._f_stock_id._base =
                UTILS::fromString<F_Fixnum::Base>(res[i]["stock_id"]);
            ret[i]._f_year._base =
                UTILS::fromString<F_Fixnum::Base>(res[i]["year"]);
            ret[i]._f_shares._base =
                UTILS::fromString<F_String::Base>(res[i]["shares"]);
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


    std::vector<O_Numshare> select(const Condition& c)
    {
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        return select(c1, "");
    }

    std::vector<O_Numshare> 
    select(const std::string& additional)
    {
        return select(_constraint, additional);
    }

    std::vector<O_Numshare> select()
    {
        return select(_constraint, "");
    }

    std::pair<O_Numshare, bool> select(const I_Numshare& id){
        return first(E_id() == id);
    }

    std::pair<O_Numshare, bool> first(const Condition& c)
    {
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        std::vector<O_Numshare> r = select(c, "limit 1");
        if(r.size() > 0)
            return std::make_pair(r[0], true);
        else
            return std::make_pair(O_Numshare(), false);
    }

    std::pair<O_Numshare, bool> first(){
        return first(_constraint);
    }

    bool insertAllFields(std::vector<O_Numshare>::const_iterator begin,
                         std::vector<O_Numshare>::const_iterator end)
    {
	std::vector<std::string> fields;
                

        fields.push_back(std::string("stock_id"));
        

        fields.push_back(std::string("year"));
        

        fields.push_back(std::string("shares"));
        

        fields.push_back(std::string("created_at"));
        

        fields.push_back(std::string("updated_at"));
	std::vector<std::vector<std::string> > rows;
	for (; begin != end; ++begin){
	    const O_Numshare& r = *begin;
	    std::vector<std::string> row;
                        
            row.push_back(toSQLString(r._stock_id()));
            
            row.push_back(toSQLString(r._year()));
            
            row.push_back(toSQLString(r._shares()));
            
            row.push_back(toSQLString(r._created_at()));
            
            row.push_back(toSQLString(r._updated_at()));
	    rows.push_back(row);
	}
        return DBFace::instance()->insert("numshares",
					  fields, rows);
    }

    bool insertDirtyFields(std::vector<O_Numshare>::const_iterator begin,
                           std::vector<O_Numshare>::const_iterator end)
    {
        if (begin == end)
            return true;
        FieldsToRows fields2Rows;
	for (std::vector<O_Numshare>::const_iterator it = begin; 
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
            ret = DBFace::instance()->insert("numshares",
                                             it->first, it->second) && ret;
        }
        return ret;
    }


    Condition                 _constraint;    
    std::vector<std::string>  _tables;
};

} //namespace DMMM
#endif //T_NUMSHARE
