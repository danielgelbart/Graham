
#ifndef T_EP
#define T_EP
#include "O_Ep.hpp"

namespace DMMM {

class T_Ep{
public:

    struct Condition{
        bool nil() const { return _cond.size() == 0; }
        std::string _cond;
    };

    T_Ep() 
    {
        _tables.push_back("eps");
    }
    T_Ep(const Condition& c) 
        : _constraint(c)
    {
        _tables.push_back("eps");
    }

 
    T_Ep(const I_Stock& parentId)
    {
        _tables.push_back("eps");
        
        _constraint._cond = "(eps.stock_id = " + parentId.to_s() + ")";
    } 
    
    struct E_id{
        E_id() 
        {  
            _field = "eps.id";
        }
        std::string _field;
        typedef T_Ep::Condition ConditionType;
        typedef Field<I_Ep>::Base ComparerType;
    };

    static E_id _id(){ 
        return E_id();
    }
    struct E_year{
        E_year() 
        {  
            _field = "eps.year";
        }
        std::string _field;
        typedef T_Ep::Condition ConditionType;
        typedef F_Fixnum::Base ComparerType;
    };

    static E_year _year(){ 
        return E_year();
    }
    struct E_source{
        E_source() 
        {  
            _field = "eps.source";
        }
        std::string _field;
        typedef T_Ep::Condition ConditionType;
        typedef F_String::Base ComparerType;
    };

    static E_source _source(){ 
        return E_source();
    }
    struct E_eps{
        E_eps() 
        {  
            _field = "eps.eps";
        }
        std::string _field;
        typedef T_Ep::Condition ConditionType;
        typedef F_BigDecimal::Base ComparerType;
    };

    static E_eps _eps(){ 
        return E_eps();
    }
    struct E_stock_id{
        E_stock_id() 
        {  
            _field = "eps.stock_id";
        }
        std::string _field;
        typedef T_Ep::Condition ConditionType;
        typedef F_Fixnum::Base ComparerType;
    };

    static E_stock_id _stock_id(){ 
        return E_stock_id();
    }
    struct E_created_at{
        E_created_at() 
        {  
            _field = "eps.created_at";
        }
        std::string _field;
        typedef T_Ep::Condition ConditionType;
        typedef F_Time::Base ComparerType;
    };

    static E_created_at _created_at(){ 
        return E_created_at();
    }
    struct E_updated_at{
        E_updated_at() 
        {  
            _field = "eps.updated_at";
        }
        std::string _field;
        typedef T_Ep::Condition ConditionType;
        typedef F_Time::Base ComparerType;
    };

    static E_updated_at _updated_at(){ 
        return E_updated_at();
    }
    struct E_revenue{
        E_revenue() 
        {  
            _field = "eps.revenue";
        }
        std::string _field;
        typedef T_Ep::Condition ConditionType;
        typedef F_String::Base ComparerType;
    };

    static E_revenue _revenue(){ 
        return E_revenue();
    }
    struct E_net_income{
        E_net_income() 
        {  
            _field = "eps.net_income";
        }
        std::string _field;
        typedef T_Ep::Condition ConditionType;
        typedef F_String::Base ComparerType;
    };

    static E_net_income _net_income(){ 
        return E_net_income();
    }
    struct E_quarter{
        E_quarter() 
        {  
            _field = "eps.quarter";
        }
        std::string _field;
        typedef T_Ep::Condition ConditionType;
        typedef F_Fixnum::Base ComparerType;
    };

    static E_quarter _quarter(){ 
        return E_quarter();
    }
    struct E_report_date{
        E_report_date() 
        {  
            _field = "eps.report_date";
        }
        std::string _field;
        typedef T_Ep::Condition ConditionType;
        typedef F_Time::Base ComparerType;
    };

    static E_report_date _report_date(){ 
        return E_report_date();
    }
    struct E_shares{
        E_shares() 
        {  
            _field = "eps.shares";
        }
        std::string _field;
        typedef T_Ep::Condition ConditionType;
        typedef F_String::Base ComparerType;
    };

    static E_shares _shares(){ 
        return E_shares();
    }
    

    std::vector<std::string> getFields()
    {
        std::vector<std::string> ret;
        ret.clear();
        ret.push_back("id");
        ret.push_back("year");
        ret.push_back("source");
        ret.push_back("eps");
        ret.push_back("stock_id");
        ret.push_back("created_at");
        ret.push_back("updated_at");
        ret.push_back("revenue");
        ret.push_back("net_income");
        ret.push_back("quarter");
        ret.push_back("report_date");
        ret.push_back("shares");
        return ret;
    }

    std::vector<O_Ep> 
    select(const Condition& c, const std::string& additional)
    {
        std::vector<O_Ep> ret;
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        std::vector<std::string> fields = getFields();
        QueryRes res;
        DBFace::instance()->select(_tables, fields, c1._cond, 
                                   additional, res);
        ret.resize(res.size());
        for(size_t i = 0; i < res.size(); ++i){
            ret[i]._f_id._base =
                UTILS::fromString<Field<I_Ep>::Base>(res[i]["id"]);
            ret[i]._f_year._base =
                UTILS::fromString<F_Fixnum::Base>(res[i]["year"]);
            ret[i]._f_source._base =
                UTILS::fromString<F_String::Base>(res[i]["source"]);
            ret[i]._f_eps._base =
                UTILS::fromString<F_BigDecimal::Base>(res[i]["eps"]);
            ret[i]._f_stock_id._base =
                UTILS::fromString<F_Fixnum::Base>(res[i]["stock_id"]);
            ret[i]._f_created_at._base =
                UTILS::fromString<F_Time::Base>(res[i]["created_at"]);
            ret[i]._f_updated_at._base =
                UTILS::fromString<F_Time::Base>(res[i]["updated_at"]);
            ret[i]._f_revenue._base =
                UTILS::fromString<F_String::Base>(res[i]["revenue"]);
            ret[i]._f_net_income._base =
                UTILS::fromString<F_String::Base>(res[i]["net_income"]);
            ret[i]._f_quarter._base =
                UTILS::fromString<F_Fixnum::Base>(res[i]["quarter"]);
            ret[i]._f_report_date._base =
                UTILS::fromString<F_Time::Base>(res[i]["report_date"]);
            ret[i]._f_shares._base =
                UTILS::fromString<F_String::Base>(res[i]["shares"]);
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


    std::vector<O_Ep> select(const Condition& c)
    {
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        return select(c1, "");
    }

    std::vector<O_Ep> 
    select(const std::string& additional)
    {
        return select(_constraint, additional);
    }

    std::vector<O_Ep> select()
    {
        return select(_constraint, "");
    }

    std::pair<O_Ep, bool> select(const I_Ep& id){
        return first(E_id() == id);
    }

    std::pair<O_Ep, bool> first(const Condition& c)
    {
        Condition c1 = _constraint.nil() ? c : _constraint && c;
        std::vector<O_Ep> r = select(c, "limit 1");
        if(r.size() > 0)
            return std::make_pair(r[0], true);
        else
            return std::make_pair(O_Ep(), false);
    }

    std::pair<O_Ep, bool> first(){
        return first(_constraint);
    }

    bool insertAllFields(std::vector<O_Ep>::const_iterator begin,
                         std::vector<O_Ep>::const_iterator end)
    {
	std::vector<std::string> fields;
                

        fields.push_back(std::string("year"));
        

        fields.push_back(std::string("source"));
        

        fields.push_back(std::string("eps"));
        

        fields.push_back(std::string("stock_id"));
        

        fields.push_back(std::string("created_at"));
        

        fields.push_back(std::string("updated_at"));
        

        fields.push_back(std::string("revenue"));
        

        fields.push_back(std::string("net_income"));
        

        fields.push_back(std::string("quarter"));
        

        fields.push_back(std::string("report_date"));
        

        fields.push_back(std::string("shares"));
	std::vector<std::vector<std::string> > rows;
	for (; begin != end; ++begin){
	    const O_Ep& r = *begin;
	    std::vector<std::string> row;
                        
            row.push_back(toSQLString(r._year()));
            
            row.push_back(toSQLString(r._source()));
            
            row.push_back(toSQLString(r._eps()));
            
            row.push_back(toSQLString(r._stock_id()));
            
            row.push_back(toSQLString(r._created_at()));
            
            row.push_back(toSQLString(r._updated_at()));
            
            row.push_back(toSQLString(r._revenue()));
            
            row.push_back(toSQLString(r._net_income()));
            
            row.push_back(toSQLString(r._quarter()));
            
            row.push_back(toSQLString(r._report_date()));
            
            row.push_back(toSQLString(r._shares()));
	    rows.push_back(row);
	}
        return DBFace::instance()->insert("eps",
					  fields, rows);
    }

    bool insertDirtyFields(std::vector<O_Ep>::const_iterator begin,
                           std::vector<O_Ep>::const_iterator end)
    {
        if (begin == end)
            return true;
        FieldsToRows fields2Rows;
	for (std::vector<O_Ep>::const_iterator it = begin; 
             it != end; ++it)
        {
            std::vector<std::string> fields;
            std::vector<std::string> row;
                        

            if (it->_f_year._dirty){
                fields.push_back(std::string("year"));
                row.push_back(toSQLString(it->_year()));
            }
            

            if (it->_f_source._dirty){
                fields.push_back(std::string("source"));
                row.push_back(toSQLString(it->_source()));
            }
            

            if (it->_f_eps._dirty){
                fields.push_back(std::string("eps"));
                row.push_back(toSQLString(it->_eps()));
            }
            

            if (it->_f_stock_id._dirty){
                fields.push_back(std::string("stock_id"));
                row.push_back(toSQLString(it->_stock_id()));
            }
            

            if (it->_f_created_at._dirty){
                fields.push_back(std::string("created_at"));
                row.push_back(toSQLString(it->_created_at()));
            }
            

            if (it->_f_updated_at._dirty){
                fields.push_back(std::string("updated_at"));
                row.push_back(toSQLString(it->_updated_at()));
            }
            

            if (it->_f_revenue._dirty){
                fields.push_back(std::string("revenue"));
                row.push_back(toSQLString(it->_revenue()));
            }
            

            if (it->_f_net_income._dirty){
                fields.push_back(std::string("net_income"));
                row.push_back(toSQLString(it->_net_income()));
            }
            

            if (it->_f_quarter._dirty){
                fields.push_back(std::string("quarter"));
                row.push_back(toSQLString(it->_quarter()));
            }
            

            if (it->_f_report_date._dirty){
                fields.push_back(std::string("report_date"));
                row.push_back(toSQLString(it->_report_date()));
            }
            

            if (it->_f_shares._dirty){
                fields.push_back(std::string("shares"));
                row.push_back(toSQLString(it->_shares()));
            }
            fields2Rows[fields].push_back(row);
	}
        bool ret = true;
        for (FieldsToRows::const_iterator it = fields2Rows.begin();
             it != fields2Rows.end(); ++it)
        {
            ret = DBFace::instance()->insert("eps",
                                             it->first, it->second) && ret;
        }
        return ret;
    }


    Condition                 _constraint;    
    std::vector<std::string>  _tables;
};

} //namespace DMMM
#endif //T_EP
