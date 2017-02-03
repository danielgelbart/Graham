
#ifndef T_SPEARNING
#define T_SPEARNING
#include "O_SpEarning.hpp"

namespace DMMM {

class T_SpEarning{
public:

    struct Condition{
        bool nil() const { return _cond.size() == 0; }
        std::string _cond;
    };

    T_SpEarning() 
    {
        _tables.push_back("sp_earnings");
    }
    T_SpEarning(const Condition& c) 
        : _constraint(c)
    {
        _tables.push_back("sp_earnings");
    }

    
    struct E_id{
        E_id() 
        {  
            _field = "sp_earnings.id";
        }
        std::string _field;
        typedef T_SpEarning::Condition ConditionType;
        typedef Field<I_SpEarning>::Base ComparerType;
    };

    static E_id _id(){ 
        return E_id();
    }
    struct E_calc_date{
        E_calc_date() 
        {  
            _field = "sp_earnings.calc_date";
        }
        std::string _field;
        typedef T_SpEarning::Condition ConditionType;
        typedef F_Date::Base ComparerType;
    };

    static E_calc_date _calc_date(){ 
        return E_calc_date();
    }
    struct E_list_file{
        E_list_file() 
        {  
            _field = "sp_earnings.list_file";
        }
        std::string _field;
        typedef T_SpEarning::Condition ConditionType;
        typedef F_String::Base ComparerType;
    };

    static E_list_file _list_file(){ 
        return E_list_file();
    }
    struct E_num_included{
        E_num_included() 
        {  
            _field = "sp_earnings.num_included";
        }
        std::string _field;
        typedef T_SpEarning::Condition ConditionType;
        typedef F_Fixnum::Base ComparerType;
    };

    static E_num_included _num_included(){ 
        return E_num_included();
    }
    struct E_excluded_list{
        E_excluded_list() 
        {  
            _field = "sp_earnings.excluded_list";
        }
        std::string _field;
        typedef T_SpEarning::Condition ConditionType;
        typedef F_String::Base ComparerType;
    };

    static E_excluded_list _excluded_list(){ 
        return E_excluded_list();
    }
    struct E_total_market_cap{
        E_total_market_cap() 
        {  
            _field = "sp_earnings.total_market_cap";
        }
        std::string _field;
        typedef T_SpEarning::Condition ConditionType;
        typedef F_String::Base ComparerType;
    };

    static E_total_market_cap _total_market_cap(){ 
        return E_total_market_cap();
    }
    struct E_public_market_cap{
        E_public_market_cap() 
        {  
            _field = "sp_earnings.public_market_cap";
        }
        std::string _field;
        typedef T_SpEarning::Condition ConditionType;
        typedef F_String::Base ComparerType;
    };

    static E_public_market_cap _public_market_cap(){ 
        return E_public_market_cap();
    }
    struct E_total_earnings{
        E_total_earnings() 
        {  
            _field = "sp_earnings.total_earnings";
        }
        std::string _field;
        typedef T_SpEarning::Condition ConditionType;
        typedef F_String::Base ComparerType;
    };

    static E_total_earnings _total_earnings(){ 
        return E_total_earnings();
    }
    struct E_market_pe{
        E_market_pe() 
        {  
            _field = "sp_earnings.market_pe";
        }
        std::string _field;
        typedef T_SpEarning::Condition ConditionType;
        typedef F_BigDecimal::Base ComparerType;
    };

    static E_market_pe _market_pe(){ 
        return E_market_pe();
    }
    struct E_index_price{
        E_index_price() 
        {  
            _field = "sp_earnings.index_price";
        }
        std::string _field;
        typedef T_SpEarning::Condition ConditionType;
        typedef F_Float::Base ComparerType;
    };

    static E_index_price _index_price(){ 
        return E_index_price();
    }
    struct E_inferred_divisor{
        E_inferred_divisor() 
        {  
            _field = "sp_earnings.inferred_divisor";
        }
        std::string _field;
        typedef T_SpEarning::Condition ConditionType;
        typedef F_Fixnum::Base ComparerType;
    };

    static E_inferred_divisor _inferred_divisor(){ 
        return E_inferred_divisor();
    }
    struct E_divisor_earnings{
        E_divisor_earnings() 
        {  
            _field = "sp_earnings.divisor_earnings";
        }
        std::string _field;
        typedef T_SpEarning::Condition ConditionType;
        typedef F_BigDecimal::Base ComparerType;
    };

    static E_divisor_earnings _divisor_earnings(){ 
        return E_divisor_earnings();
    }
    struct E_divisor_pe{
        E_divisor_pe() 
        {  
            _field = "sp_earnings.divisor_pe";
        }
        std::string _field;
        typedef T_SpEarning::Condition ConditionType;
        typedef F_BigDecimal::Base ComparerType;
    };

    static E_divisor_pe _divisor_pe(){ 
        return E_divisor_pe();
    }
    struct E_notes{
        E_notes() 
        {  
            _field = "sp_earnings.notes";
        }
        std::string _field;
        typedef T_SpEarning::Condition ConditionType;
        typedef F_String::Base ComparerType;
    };

    static E_notes _notes(){ 
        return E_notes();
    }
    struct E_created_at{
        E_created_at() 
        {  
            _field = "sp_earnings.created_at";
        }
        std::string _field;
        typedef T_SpEarning::Condition ConditionType;
        typedef F_Time::Base ComparerType;
    };

    static E_created_at _created_at(){ 
        return E_created_at();
    }
    struct E_updated_at{
        E_updated_at() 
        {  
            _field = "sp_earnings.updated_at";
        }
        std::string _field;
        typedef T_SpEarning::Condition ConditionType;
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
        ret.push_back("calc_date");
        ret.push_back("list_file");
        ret.push_back("num_included");
        ret.push_back("excluded_list");
        ret.push_back("total_market_cap");
        ret.push_back("public_market_cap");
        ret.push_back("total_earnings");
        ret.push_back("market_pe");
        ret.push_back("index_price");
        ret.push_back("inferred_divisor");
        ret.push_back("divisor_earnings");
        ret.push_back("divisor_pe");
        ret.push_back("notes");
        ret.push_back("created_at");
        ret.push_back("updated_at");
        return ret;
    }

    std::vector<O_SpEarning> 
    select(const Condition& c, const std::string& additional)
    {
        std::vector<O_SpEarning> ret;
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        std::vector<std::string> fields = getFields();
        QueryRes res;
        DBFace::instance()->select(_tables, fields, c1._cond, 
                                   additional, res);
        ret.resize(res.size());
        for(size_t i = 0; i < res.size(); ++i){
            ret[i]._f_id._base =
                UTILS::fromString<Field<I_SpEarning>::Base>(res[i]["id"]);
            ret[i]._f_calc_date._base =
                UTILS::fromString<F_Date::Base>(res[i]["calc_date"]);
            ret[i]._f_list_file._base =
                UTILS::fromString<F_String::Base>(res[i]["list_file"]);
            ret[i]._f_num_included._base =
                UTILS::fromString<F_Fixnum::Base>(res[i]["num_included"]);
            ret[i]._f_excluded_list._base =
                UTILS::fromString<F_String::Base>(res[i]["excluded_list"]);
            ret[i]._f_total_market_cap._base =
                UTILS::fromString<F_String::Base>(res[i]["total_market_cap"]);
            ret[i]._f_public_market_cap._base =
                UTILS::fromString<F_String::Base>(res[i]["public_market_cap"]);
            ret[i]._f_total_earnings._base =
                UTILS::fromString<F_String::Base>(res[i]["total_earnings"]);
            ret[i]._f_market_pe._base =
                UTILS::fromString<F_BigDecimal::Base>(res[i]["market_pe"]);
            ret[i]._f_index_price._base =
                UTILS::fromString<F_Float::Base>(res[i]["index_price"]);
            ret[i]._f_inferred_divisor._base =
                UTILS::fromString<F_Fixnum::Base>(res[i]["inferred_divisor"]);
            ret[i]._f_divisor_earnings._base =
                UTILS::fromString<F_BigDecimal::Base>(res[i]["divisor_earnings"]);
            ret[i]._f_divisor_pe._base =
                UTILS::fromString<F_BigDecimal::Base>(res[i]["divisor_pe"]);
            ret[i]._f_notes._base =
                UTILS::fromString<F_String::Base>(res[i]["notes"]);
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


    std::vector<O_SpEarning> select(const Condition& c)
    {
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        return select(c1, "");
    }

    std::vector<O_SpEarning> 
    select(const std::string& additional)
    {
        return select(_constraint, additional);
    }

    std::vector<O_SpEarning> select()
    {
        return select(_constraint, "");
    }

    std::pair<O_SpEarning, bool> select(const I_SpEarning& id){
        return first(E_id() == id);
    }

    std::pair<O_SpEarning, bool> first(const Condition& c)
    {
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        std::vector<O_SpEarning> r = select(c, "limit 1");
        if(r.size() > 0)
            return std::make_pair(r[0], true);
        else
            return std::make_pair(O_SpEarning(), false);
    }

    std::pair<O_SpEarning, bool> first(){
        return first(_constraint);
    }

    bool insertAllFields(std::vector<O_SpEarning>::const_iterator begin,
                         std::vector<O_SpEarning>::const_iterator end)
    {
	std::vector<std::string> fields;
                

        fields.push_back(std::string("calc_date"));
        

        fields.push_back(std::string("list_file"));
        

        fields.push_back(std::string("num_included"));
        

        fields.push_back(std::string("excluded_list"));
        

        fields.push_back(std::string("total_market_cap"));
        

        fields.push_back(std::string("public_market_cap"));
        

        fields.push_back(std::string("total_earnings"));
        

        fields.push_back(std::string("market_pe"));
        

        fields.push_back(std::string("index_price"));
        

        fields.push_back(std::string("inferred_divisor"));
        

        fields.push_back(std::string("divisor_earnings"));
        

        fields.push_back(std::string("divisor_pe"));
        

        fields.push_back(std::string("notes"));
        

        fields.push_back(std::string("created_at"));
        

        fields.push_back(std::string("updated_at"));
	std::vector<std::vector<std::string> > rows;
	for (; begin != end; ++begin){
	    const O_SpEarning& r = *begin;
	    std::vector<std::string> row;
                        
            row.push_back(toSQLString(r._calc_date()));
            
            row.push_back(toSQLString(r._list_file()));
            
            row.push_back(toSQLString(r._num_included()));
            
            row.push_back(toSQLString(r._excluded_list()));
            
            row.push_back(toSQLString(r._total_market_cap()));
            
            row.push_back(toSQLString(r._public_market_cap()));
            
            row.push_back(toSQLString(r._total_earnings()));
            
            row.push_back(toSQLString(r._market_pe()));
            
            row.push_back(toSQLString(r._index_price()));
            
            row.push_back(toSQLString(r._inferred_divisor()));
            
            row.push_back(toSQLString(r._divisor_earnings()));
            
            row.push_back(toSQLString(r._divisor_pe()));
            
            row.push_back(toSQLString(r._notes()));
            
            row.push_back(toSQLString(r._created_at()));
            
            row.push_back(toSQLString(r._updated_at()));
	    rows.push_back(row);
	}
        return DBFace::instance()->insert("sp_earnings",
					  fields, rows);
    }

    bool insertDirtyFields(std::vector<O_SpEarning>::const_iterator begin,
                           std::vector<O_SpEarning>::const_iterator end)
    {
        if (begin == end)
            return true;
        FieldsToRows fields2Rows;
	for (std::vector<O_SpEarning>::const_iterator it = begin; 
             it != end; ++it)
        {
            std::vector<std::string> fields;
            std::vector<std::string> row;
                        

            if (it->_f_calc_date._dirty){
                fields.push_back(std::string("calc_date"));
                row.push_back(toSQLString(it->_calc_date()));
            }
            

            if (it->_f_list_file._dirty){
                fields.push_back(std::string("list_file"));
                row.push_back(toSQLString(it->_list_file()));
            }
            

            if (it->_f_num_included._dirty){
                fields.push_back(std::string("num_included"));
                row.push_back(toSQLString(it->_num_included()));
            }
            

            if (it->_f_excluded_list._dirty){
                fields.push_back(std::string("excluded_list"));
                row.push_back(toSQLString(it->_excluded_list()));
            }
            

            if (it->_f_total_market_cap._dirty){
                fields.push_back(std::string("total_market_cap"));
                row.push_back(toSQLString(it->_total_market_cap()));
            }
            

            if (it->_f_public_market_cap._dirty){
                fields.push_back(std::string("public_market_cap"));
                row.push_back(toSQLString(it->_public_market_cap()));
            }
            

            if (it->_f_total_earnings._dirty){
                fields.push_back(std::string("total_earnings"));
                row.push_back(toSQLString(it->_total_earnings()));
            }
            

            if (it->_f_market_pe._dirty){
                fields.push_back(std::string("market_pe"));
                row.push_back(toSQLString(it->_market_pe()));
            }
            

            if (it->_f_index_price._dirty){
                fields.push_back(std::string("index_price"));
                row.push_back(toSQLString(it->_index_price()));
            }
            

            if (it->_f_inferred_divisor._dirty){
                fields.push_back(std::string("inferred_divisor"));
                row.push_back(toSQLString(it->_inferred_divisor()));
            }
            

            if (it->_f_divisor_earnings._dirty){
                fields.push_back(std::string("divisor_earnings"));
                row.push_back(toSQLString(it->_divisor_earnings()));
            }
            

            if (it->_f_divisor_pe._dirty){
                fields.push_back(std::string("divisor_pe"));
                row.push_back(toSQLString(it->_divisor_pe()));
            }
            

            if (it->_f_notes._dirty){
                fields.push_back(std::string("notes"));
                row.push_back(toSQLString(it->_notes()));
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
            ret = DBFace::instance()->insert("sp_earnings",
                                             it->first, it->second) && ret;
        }
        return ret;
    }


    Condition                 _constraint;    
    std::vector<std::string>  _tables;
};

} //namespace DMMM
#endif //T_SPEARNING
