#ifndef O_SPEARNING
#define O_SPEARNING

#include <map>
#include <string>
#include "dmmm_utils.hpp"
#include "dmmm_dbface.h"
#include "dmmm_identifiers.hpp"
#include "dmmm_fields.hpp"
#include "dmmm_comparators.hpp"


namespace DMMM {

class O_SpEarning{
public:

    O_SpEarning() {}

O_SpEarning(const I_SpEarning& id) 
        : _f_id(id)
        {}



    const Field<I_SpEarning>::Base& _id() const { 
        return _f_id._base; 
    }
    Field<I_SpEarning>::Base& _id() { 
        _f_id._dirty = true; 
        return _f_id._base; 
    }
    const F_Date::Base& _calc_date() const { 
        return _f_calc_date._base; 
    }
    F_Date::Base& _calc_date() { 
        _f_calc_date._dirty = true; 
        return _f_calc_date._base; 
    }
    const F_String::Base& _list_file() const { 
        return _f_list_file._base; 
    }
    F_String::Base& _list_file() { 
        _f_list_file._dirty = true; 
        return _f_list_file._base; 
    }
    const F_Fixnum::Base& _num_included() const { 
        return _f_num_included._base; 
    }
    F_Fixnum::Base& _num_included() { 
        _f_num_included._dirty = true; 
        return _f_num_included._base; 
    }
    const F_String::Base& _excluded_list() const { 
        return _f_excluded_list._base; 
    }
    F_String::Base& _excluded_list() { 
        _f_excluded_list._dirty = true; 
        return _f_excluded_list._base; 
    }
    const F_String::Base& _total_market_cap() const { 
        return _f_total_market_cap._base; 
    }
    F_String::Base& _total_market_cap() { 
        _f_total_market_cap._dirty = true; 
        return _f_total_market_cap._base; 
    }
    const F_String::Base& _public_market_cap() const { 
        return _f_public_market_cap._base; 
    }
    F_String::Base& _public_market_cap() { 
        _f_public_market_cap._dirty = true; 
        return _f_public_market_cap._base; 
    }
    const F_String::Base& _total_earnings() const { 
        return _f_total_earnings._base; 
    }
    F_String::Base& _total_earnings() { 
        _f_total_earnings._dirty = true; 
        return _f_total_earnings._base; 
    }
    const F_BigDecimal::Base& _market_pe() const { 
        return _f_market_pe._base; 
    }
    F_BigDecimal::Base& _market_pe() { 
        _f_market_pe._dirty = true; 
        return _f_market_pe._base; 
    }
    const F_Float::Base& _index_price() const { 
        return _f_index_price._base; 
    }
    F_Float::Base& _index_price() { 
        _f_index_price._dirty = true; 
        return _f_index_price._base; 
    }
    const F_Fixnum::Base& _inferred_divisor() const { 
        return _f_inferred_divisor._base; 
    }
    F_Fixnum::Base& _inferred_divisor() { 
        _f_inferred_divisor._dirty = true; 
        return _f_inferred_divisor._base; 
    }
    const F_BigDecimal::Base& _divisor_earnings() const { 
        return _f_divisor_earnings._base; 
    }
    F_BigDecimal::Base& _divisor_earnings() { 
        _f_divisor_earnings._dirty = true; 
        return _f_divisor_earnings._base; 
    }
    const F_BigDecimal::Base& _divisor_pe() const { 
        return _f_divisor_pe._base; 
    }
    F_BigDecimal::Base& _divisor_pe() { 
        _f_divisor_pe._dirty = true; 
        return _f_divisor_pe._base; 
    }
    const F_String::Base& _notes() const { 
        return _f_notes._base; 
    }
    F_String::Base& _notes() { 
        _f_notes._dirty = true; 
        return _f_notes._base; 
    }
    const F_Time::Base& _created_at() const { 
        return _f_created_at._base; 
    }
    F_Time::Base& _created_at() { 
        _f_created_at._dirty = true; 
        return _f_created_at._base; 
    }
    const F_Time::Base& _updated_at() const { 
        return _f_updated_at._base; 
    }
    F_Time::Base& _updated_at() { 
        _f_updated_at._dirty = true; 
        return _f_updated_at._base; 
    }

    bool update(){
        std::map<std::string, std::string> field2Val;
        if (_f_id._dirty)
            field2Val["id"] = 
                toSQLString(_f_id._base);
        if (_f_calc_date._dirty)
            field2Val["calc_date"] = 
                toSQLString(_f_calc_date._base);
        if (_f_list_file._dirty)
            field2Val["list_file"] = 
                toSQLString(_f_list_file._base);
        if (_f_num_included._dirty)
            field2Val["num_included"] = 
                toSQLString(_f_num_included._base);
        if (_f_excluded_list._dirty)
            field2Val["excluded_list"] = 
                toSQLString(_f_excluded_list._base);
        if (_f_total_market_cap._dirty)
            field2Val["total_market_cap"] = 
                toSQLString(_f_total_market_cap._base);
        if (_f_public_market_cap._dirty)
            field2Val["public_market_cap"] = 
                toSQLString(_f_public_market_cap._base);
        if (_f_total_earnings._dirty)
            field2Val["total_earnings"] = 
                toSQLString(_f_total_earnings._base);
        if (_f_market_pe._dirty)
            field2Val["market_pe"] = 
                toSQLString(_f_market_pe._base);
        if (_f_index_price._dirty)
            field2Val["index_price"] = 
                toSQLString(_f_index_price._base);
        if (_f_inferred_divisor._dirty)
            field2Val["inferred_divisor"] = 
                toSQLString(_f_inferred_divisor._base);
        if (_f_divisor_earnings._dirty)
            field2Val["divisor_earnings"] = 
                toSQLString(_f_divisor_earnings._base);
        if (_f_divisor_pe._dirty)
            field2Val["divisor_pe"] = 
                toSQLString(_f_divisor_pe._base);
        if (_f_notes._dirty)
            field2Val["notes"] = 
                toSQLString(_f_notes._base);
        if (_f_created_at._dirty)
            field2Val["created_at"] = 
                toSQLString(_f_created_at._base);

        field2Val["updated_at"] = "NOW()";

        std::string where =
            "id=" + toSQLString(_f_id._base);
        if (DBFace::instance()->update("sp_earnings", 
                                       field2Val, where))
        {
            _f_id._dirty = false;
            _f_calc_date._dirty = false;
            _f_list_file._dirty = false;
            _f_num_included._dirty = false;
            _f_excluded_list._dirty = false;
            _f_total_market_cap._dirty = false;
            _f_public_market_cap._dirty = false;
            _f_total_earnings._dirty = false;
            _f_market_pe._dirty = false;
            _f_index_price._dirty = false;
            _f_inferred_divisor._dirty = false;
            _f_divisor_earnings._dirty = false;
            _f_divisor_pe._dirty = false;
            _f_notes._dirty = false;
            _f_created_at._dirty = false;
            _f_updated_at._dirty = false;
            return true;
        }
        else
            return false;
    }

    bool insert(){
        std::map<std::string, std::string> field2Val;
        if (_f_id._dirty)
            field2Val["id"] = 
                toSQLString(_f_id._base);
        if (_f_calc_date._dirty)
            field2Val["calc_date"] = 
                toSQLString(_f_calc_date._base);
        if (_f_list_file._dirty)
            field2Val["list_file"] = 
                toSQLString(_f_list_file._base);
        if (_f_num_included._dirty)
            field2Val["num_included"] = 
                toSQLString(_f_num_included._base);
        if (_f_excluded_list._dirty)
            field2Val["excluded_list"] = 
                toSQLString(_f_excluded_list._base);
        if (_f_total_market_cap._dirty)
            field2Val["total_market_cap"] = 
                toSQLString(_f_total_market_cap._base);
        if (_f_public_market_cap._dirty)
            field2Val["public_market_cap"] = 
                toSQLString(_f_public_market_cap._base);
        if (_f_total_earnings._dirty)
            field2Val["total_earnings"] = 
                toSQLString(_f_total_earnings._base);
        if (_f_market_pe._dirty)
            field2Val["market_pe"] = 
                toSQLString(_f_market_pe._base);
        if (_f_index_price._dirty)
            field2Val["index_price"] = 
                toSQLString(_f_index_price._base);
        if (_f_inferred_divisor._dirty)
            field2Val["inferred_divisor"] = 
                toSQLString(_f_inferred_divisor._base);
        if (_f_divisor_earnings._dirty)
            field2Val["divisor_earnings"] = 
                toSQLString(_f_divisor_earnings._base);
        if (_f_divisor_pe._dirty)
            field2Val["divisor_pe"] = 
                toSQLString(_f_divisor_pe._base);
        if (_f_notes._dirty)
            field2Val["notes"] = 
                toSQLString(_f_notes._base);

        field2Val["created_at"] = "NOW()";

        if (_f_updated_at._dirty)
            field2Val["updated_at"] = 
                toSQLString(_f_updated_at._base);

        
        if (DBFace::instance()->
                insert("sp_earnings", field2Val,
                       _f_id._base.serialization()))
        {
            _f_id._dirty = false;
            _f_calc_date._dirty = false;
            _f_list_file._dirty = false;
            _f_num_included._dirty = false;
            _f_excluded_list._dirty = false;
            _f_total_market_cap._dirty = false;
            _f_public_market_cap._dirty = false;
            _f_total_earnings._dirty = false;
            _f_market_pe._dirty = false;
            _f_index_price._dirty = false;
            _f_inferred_divisor._dirty = false;
            _f_divisor_earnings._dirty = false;
            _f_divisor_pe._dirty = false;
            _f_notes._dirty = false;
            _f_created_at._dirty = false;
            _f_updated_at._dirty = false;
            return true;
        }
        else
            return false;
                                   
    }
private:
    Field<I_SpEarning> _f_id;
    F_Date _f_calc_date;
    F_String _f_list_file;
    F_Fixnum _f_num_included;
    F_String _f_excluded_list;
    F_String _f_total_market_cap;
    F_String _f_public_market_cap;
    F_String _f_total_earnings;
    F_BigDecimal _f_market_pe;
    F_Float _f_index_price;
    F_Fixnum _f_inferred_divisor;
    F_BigDecimal _f_divisor_earnings;
    F_BigDecimal _f_divisor_pe;
    F_String _f_notes;
    F_Time _f_created_at;
    F_Time _f_updated_at;

    friend class T_SpEarning;
};

} //namespace DMMM
#endif //O_SPEARNING
