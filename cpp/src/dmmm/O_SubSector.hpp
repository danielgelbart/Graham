#ifndef O_SUBSECTOR
#define O_SUBSECTOR

#include <map>
#include <string>
#include "dmmm_utils.hpp"
#include "dmmm_dbface.h"
#include "dmmm_identifiers.hpp"
#include "dmmm_fields.hpp"
#include "dmmm_comparators.hpp"
#include "T_Stock.hpp"


namespace DMMM {

class O_SubSector{
public:

    O_SubSector() {}
    O_SubSector(const I_Sector& parent_id) 
        : _f_sector_id(parent_id)
        {}

O_SubSector(const I_SubSector& id) 
        : _f_id(id)
        {}


    std::vector<O_Stock> _stocks() const
    {
        T_Stock table;
        return table.select(table._sub_sector_id() == _id());   
    }
    std::vector<O_Stock> 
        _stocks(const T_Stock::Condition& c) const
    {
        T_Stock table(c);
        return table.select(table._sub_sector_id() == _id());   
    }


    const Field<I_SubSector>::Base& _id() const { 
        return _f_id._base; 
    }
    Field<I_SubSector>::Base& _id() { 
        _f_id._dirty = true; 
        return _f_id._base; 
    }
    const F_String::Base& _name() const { 
        return _f_name._base; 
    }
    F_String::Base& _name() { 
        _f_name._dirty = true; 
        return _f_name._base; 
    }
    const F_Fixnum::Base& _sector_id() const { 
        return _f_sector_id._base; 
    }
    F_Fixnum::Base& _sector_id() { 
        _f_sector_id._dirty = true; 
        return _f_sector_id._base; 
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
        if (_f_name._dirty)
            field2Val["name"] = 
                toSQLString(_f_name._base);
        if (_f_sector_id._dirty)
            field2Val["sector_id"] = 
                toSQLString(_f_sector_id._base);
        if (_f_created_at._dirty)
            field2Val["created_at"] = 
                toSQLString(_f_created_at._base);

        field2Val["updated_at"] = "NOW()";

        std::string where =
            "id=" + toSQLString(_f_id._base);
        if (DBFace::instance()->update("sub_sectors", 
                                       field2Val, where))
        {
            _f_id._dirty = false;
            _f_name._dirty = false;
            _f_sector_id._dirty = false;
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
        if (_f_name._dirty)
            field2Val["name"] = 
                toSQLString(_f_name._base);
        if (_f_sector_id._dirty)
            field2Val["sector_id"] = 
                toSQLString(_f_sector_id._base);

        field2Val["created_at"] = "NOW()";

        if (_f_updated_at._dirty)
            field2Val["updated_at"] = 
                toSQLString(_f_updated_at._base);

        
        if (DBFace::instance()->
                insert("sub_sectors", field2Val,
                       _f_id._base.serialization()))
        {
            _f_id._dirty = false;
            _f_name._dirty = false;
            _f_sector_id._dirty = false;
            _f_created_at._dirty = false;
            _f_updated_at._dirty = false;
            return true;
        }
        else
            return false;
                                   
    }
private:
    Field<I_SubSector> _f_id;
    F_String _f_name;
    F_Fixnum _f_sector_id;
    F_Time _f_created_at;
    F_Time _f_updated_at;

    friend class T_SubSector;
};

} //namespace DMMM
#endif //O_SUBSECTOR
