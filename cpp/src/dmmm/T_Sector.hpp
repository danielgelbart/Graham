
#ifndef T_SECTOR
#define T_SECTOR
#include "O_Sector.hpp"

namespace DMMM {

class T_Sector{
public:

    struct Condition{
        bool nil() const { return _cond.size() == 0; }
        std::string _cond;
    };

    T_Sector() 
    {
        _tables.push_back("sectors");
    }
    T_Sector(const Condition& c) 
        : _constraint(c)
    {
        _tables.push_back("sectors");
    }

    
    struct E_id{
        E_id() 
        {  
            _field = "sectors.id";
        }
        std::string _field;
        typedef T_Sector::Condition ConditionType;
        typedef Field<I_Sector>::Base ComparerType;
    };

    static E_id _id(){ 
        return E_id();
    }
    struct E_name{
        E_name() 
        {  
            _field = "sectors.name";
        }
        std::string _field;
        typedef T_Sector::Condition ConditionType;
        typedef F_String::Base ComparerType;
    };

    static E_name _name(){ 
        return E_name();
    }
    

    std::vector<std::string> getFields()
    {
        std::vector<std::string> ret;
        ret.clear();
        ret.push_back("id");
        ret.push_back("name");
        return ret;
    }

    std::vector<O_Sector> 
    select(const Condition& c, const std::string& additional)
    {
        std::vector<O_Sector> ret;
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        std::vector<std::string> fields = getFields();
        QueryRes res;
        DBFace::instance()->select(_tables, fields, c1._cond, 
                                   additional, res);
        ret.resize(res.size());
        for(size_t i = 0; i < res.size(); ++i){
            ret[i]._f_id._base =
                UTILS::fromString<Field<I_Sector>::Base>(res[i]["id"]);
            ret[i]._f_name._base =
                UTILS::fromString<F_String::Base>(res[i]["name"]);
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


    std::vector<O_Sector> select(const Condition& c)
    {
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        return select(c1, "");
    }

    std::vector<O_Sector> 
    select(const std::string& additional)
    {
        return select(_constraint, additional);
    }

    std::vector<O_Sector> select()
    {
        return select(_constraint, "");
    }

    std::pair<O_Sector, bool> select(const I_Sector& id){
        return first(E_id() == id);
    }

    std::pair<O_Sector, bool> first(const Condition& c)
    {
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        std::vector<O_Sector> r = select(c, "limit 1");
        if(r.size() > 0)
            return std::make_pair(r[0], true);
        else
            return std::make_pair(O_Sector(), false);
    }

    std::pair<O_Sector, bool> first(){
        return first(_constraint);
    }

    bool insertAllFields(std::vector<O_Sector>::const_iterator begin,
                         std::vector<O_Sector>::const_iterator end)
    {
	std::vector<std::string> fields;
                

        fields.push_back(std::string("name"));
	std::vector<std::vector<std::string> > rows;
	for (; begin != end; ++begin){
	    const O_Sector& r = *begin;
	    std::vector<std::string> row;
                        
            row.push_back(toSQLString(r._name()));
	    rows.push_back(row);
	}
        return DBFace::instance()->insert("sectors",
					  fields, rows);
    }

    bool insertDirtyFields(std::vector<O_Sector>::const_iterator begin,
                           std::vector<O_Sector>::const_iterator end)
    {
        if (begin == end)
            return true;
        FieldsToRows fields2Rows;
	for (std::vector<O_Sector>::const_iterator it = begin; 
             it != end; ++it)
        {
            std::vector<std::string> fields;
            std::vector<std::string> row;
                        

            if (it->_f_name._dirty){
                fields.push_back(std::string("name"));
                row.push_back(toSQLString(it->_name()));
            }
            fields2Rows[fields].push_back(row);
	}
        bool ret = true;
        for (FieldsToRows::const_iterator it = fields2Rows.begin();
             it != fields2Rows.end(); ++it)
        {
            ret = DBFace::instance()->insert("sectors",
                                             it->first, it->second) && ret;
        }
        return ret;
    }


    Condition                 _constraint;    
    std::vector<std::string>  _tables;
};

} //namespace DMMM
#endif //T_SECTOR
