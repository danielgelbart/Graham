
#ifndef T_SHARECLASS
#define T_SHARECLASS
#include "O_ShareClass.hpp"

namespace DMMM {

class T_ShareClass{
public:

    struct Condition{
        bool nil() const { return _cond.size() == 0; }
        std::string _cond;
    };

    T_ShareClass() 
    {
        _tables.push_back("share_classes");
    }
    T_ShareClass(const Condition& c) 
        : _constraint(c)
    {
        _tables.push_back("share_classes");
    }

 
    T_ShareClass(const I_Stock& parentId)
    {
        _tables.push_back("share_classes");
        
        _constraint._cond = "(share_classes.stock_id = " + parentId.to_s() + ")";
    } 
    
    struct E_id{
        E_id() 
        {  
            _field = "share_classes.id";
        }
        std::string _field;
        typedef T_ShareClass::Condition ConditionType;
        typedef Field<I_ShareClass>::Base ComparerType;
    };

    static E_id _id(){ 
        return E_id();
    }
    struct E_stock_id{
        E_stock_id() 
        {  
            _field = "share_classes.stock_id";
        }
        std::string _field;
        typedef T_ShareClass::Condition ConditionType;
        typedef F_Fixnum::Base ComparerType;
    };

    static E_stock_id _stock_id(){ 
        return E_stock_id();
    }
    struct E_ticker{
        E_ticker() 
        {  
            _field = "share_classes.ticker";
        }
        std::string _field;
        typedef T_ShareClass::Condition ConditionType;
        typedef F_String::Base ComparerType;
    };

    static E_ticker _ticker(){ 
        return E_ticker();
    }
    struct E_sclass{
        E_sclass() 
        {  
            _field = "share_classes.sclass";
        }
        std::string _field;
        typedef T_ShareClass::Condition ConditionType;
        typedef F_String::Base ComparerType;
    };

    static E_sclass _sclass(){ 
        return E_sclass();
    }
    struct E_votes{
        E_votes() 
        {  
            _field = "share_classes.votes";
        }
        std::string _field;
        typedef T_ShareClass::Condition ConditionType;
        typedef F_Fixnum::Base ComparerType;
    };

    static E_votes _votes(){ 
        return E_votes();
    }
    struct E_nshares{
        E_nshares() 
        {  
            _field = "share_classes.nshares";
        }
        std::string _field;
        typedef T_ShareClass::Condition ConditionType;
        typedef F_String::Base ComparerType;
    };

    static E_nshares _nshares(){ 
        return E_nshares();
    }
    struct E_float_date{
        E_float_date() 
        {  
            _field = "share_classes.float_date";
        }
        std::string _field;
        typedef T_ShareClass::Condition ConditionType;
        typedef F_Date::Base ComparerType;
    };

    static E_float_date _float_date(){ 
        return E_float_date();
    }
    struct E_mul_factor{
        E_mul_factor() 
        {  
            _field = "share_classes.mul_factor";
        }
        std::string _field;
        typedef T_ShareClass::Condition ConditionType;
        typedef F_Fixnum::Base ComparerType;
    };

    static E_mul_factor _mul_factor(){ 
        return E_mul_factor();
    }
    struct E_note{
        E_note() 
        {  
            _field = "share_classes.note";
        }
        std::string _field;
        typedef T_ShareClass::Condition ConditionType;
        typedef F_String::Base ComparerType;
    };

    static E_note _note(){ 
        return E_note();
    }
    struct E_created_at{
        E_created_at() 
        {  
            _field = "share_classes.created_at";
        }
        std::string _field;
        typedef T_ShareClass::Condition ConditionType;
        typedef F_Time::Base ComparerType;
    };

    static E_created_at _created_at(){ 
        return E_created_at();
    }
    struct E_updated_at{
        E_updated_at() 
        {  
            _field = "share_classes.updated_at";
        }
        std::string _field;
        typedef T_ShareClass::Condition ConditionType;
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
        ret.push_back("ticker");
        ret.push_back("sclass");
        ret.push_back("votes");
        ret.push_back("nshares");
        ret.push_back("float_date");
        ret.push_back("mul_factor");
        ret.push_back("note");
        ret.push_back("created_at");
        ret.push_back("updated_at");
        return ret;
    }

    std::vector<O_ShareClass> 
    select(const Condition& c, const std::string& additional)
    {
        std::vector<O_ShareClass> ret;
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        std::vector<std::string> fields = getFields();
        QueryRes res;
        DBFace::instance()->select(_tables, fields, c1._cond, 
                                   additional, res);
        ret.resize(res.size());
        for(size_t i = 0; i < res.size(); ++i){
            ret[i]._f_id._base =
                UTILS::fromString<Field<I_ShareClass>::Base>(res[i]["id"]);
            ret[i]._f_stock_id._base =
                UTILS::fromString<F_Fixnum::Base>(res[i]["stock_id"]);
            ret[i]._f_ticker._base =
                UTILS::fromString<F_String::Base>(res[i]["ticker"]);
            ret[i]._f_sclass._base =
                UTILS::fromString<F_String::Base>(res[i]["sclass"]);
            ret[i]._f_votes._base =
                UTILS::fromString<F_Fixnum::Base>(res[i]["votes"]);
            ret[i]._f_nshares._base =
                UTILS::fromString<F_String::Base>(res[i]["nshares"]);
            ret[i]._f_float_date._base =
                UTILS::fromString<F_Date::Base>(res[i]["float_date"]);
            ret[i]._f_mul_factor._base =
                UTILS::fromString<F_Fixnum::Base>(res[i]["mul_factor"]);
            ret[i]._f_note._base =
                UTILS::fromString<F_String::Base>(res[i]["note"]);
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


    std::vector<O_ShareClass> select(const Condition& c)
    {
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        return select(c1, "");
    }

    std::vector<O_ShareClass> 
    select(const std::string& additional)
    {
        return select(_constraint, additional);
    }

    std::vector<O_ShareClass> select()
    {
        return select(_constraint, "");
    }

    std::pair<O_ShareClass, bool> select(const I_ShareClass& id){
        return first(E_id() == id);
    }

    std::pair<O_ShareClass, bool> first(const Condition& c)
    {
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        std::vector<O_ShareClass> r = select(c, "limit 1");
        if(r.size() > 0)
            return std::make_pair(r[0], true);
        else
            return std::make_pair(O_ShareClass(), false);
    }

    std::pair<O_ShareClass, bool> first(){
        return first(_constraint);
    }

    bool insertAllFields(std::vector<O_ShareClass>::const_iterator begin,
                         std::vector<O_ShareClass>::const_iterator end)
    {
	std::vector<std::string> fields;
                

        fields.push_back(std::string("stock_id"));
        

        fields.push_back(std::string("ticker"));
        

        fields.push_back(std::string("sclass"));
        

        fields.push_back(std::string("votes"));
        

        fields.push_back(std::string("nshares"));
        

        fields.push_back(std::string("float_date"));
        

        fields.push_back(std::string("mul_factor"));
        

        fields.push_back(std::string("note"));
        

        fields.push_back(std::string("created_at"));
        

        fields.push_back(std::string("updated_at"));
	std::vector<std::vector<std::string> > rows;
	for (; begin != end; ++begin){
	    const O_ShareClass& r = *begin;
	    std::vector<std::string> row;
                        
            row.push_back(toSQLString(r._stock_id()));
            
            row.push_back(toSQLString(r._ticker()));
            
            row.push_back(toSQLString(r._sclass()));
            
            row.push_back(toSQLString(r._votes()));
            
            row.push_back(toSQLString(r._nshares()));
            
            row.push_back(toSQLString(r._float_date()));
            
            row.push_back(toSQLString(r._mul_factor()));
            
            row.push_back(toSQLString(r._note()));
            
            row.push_back(toSQLString(r._created_at()));
            
            row.push_back(toSQLString(r._updated_at()));
	    rows.push_back(row);
	}
        return DBFace::instance()->insert("share_classes",
					  fields, rows);
    }

    bool insertDirtyFields(std::vector<O_ShareClass>::const_iterator begin,
                           std::vector<O_ShareClass>::const_iterator end)
    {
        if (begin == end)
            return true;
        FieldsToRows fields2Rows;
	for (std::vector<O_ShareClass>::const_iterator it = begin; 
             it != end; ++it)
        {
            std::vector<std::string> fields;
            std::vector<std::string> row;
                        

            if (it->_f_stock_id._dirty){
                fields.push_back(std::string("stock_id"));
                row.push_back(toSQLString(it->_stock_id()));
            }
            

            if (it->_f_ticker._dirty){
                fields.push_back(std::string("ticker"));
                row.push_back(toSQLString(it->_ticker()));
            }
            

            if (it->_f_sclass._dirty){
                fields.push_back(std::string("sclass"));
                row.push_back(toSQLString(it->_sclass()));
            }
            

            if (it->_f_votes._dirty){
                fields.push_back(std::string("votes"));
                row.push_back(toSQLString(it->_votes()));
            }
            

            if (it->_f_nshares._dirty){
                fields.push_back(std::string("nshares"));
                row.push_back(toSQLString(it->_nshares()));
            }
            

            if (it->_f_float_date._dirty){
                fields.push_back(std::string("float_date"));
                row.push_back(toSQLString(it->_float_date()));
            }
            

            if (it->_f_mul_factor._dirty){
                fields.push_back(std::string("mul_factor"));
                row.push_back(toSQLString(it->_mul_factor()));
            }
            

            if (it->_f_note._dirty){
                fields.push_back(std::string("note"));
                row.push_back(toSQLString(it->_note()));
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
            ret = DBFace::instance()->insert("share_classes",
                                             it->first, it->second) && ret;
        }
        return ret;
    }


    Condition                 _constraint;    
    std::vector<std::string>  _tables;
};

} //namespace DMMM
#endif //T_SHARECLASS
