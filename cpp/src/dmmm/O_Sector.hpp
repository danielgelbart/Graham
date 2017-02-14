#ifndef O_SECTOR
#define O_SECTOR

#include <map>
#include <string>
#include "dmmm_utils.hpp"
#include "dmmm_dbface.h"
#include "dmmm_identifiers.hpp"
#include "dmmm_fields.hpp"
#include "dmmm_comparators.hpp"
#include "T_SubSector.hpp"


namespace DMMM {

class O_Sector{
public:

    O_Sector() {}

O_Sector(const I_Sector& id) 
        : _f_id(id)
        {}


    std::vector<O_SubSector> _sub_sectors() const
    {
        T_SubSector table;
        return table.select(table._sector_id() == _id());   
    }
    std::vector<O_SubSector> 
        _sub_sectors(const T_SubSector::Condition& c) const
    {
        T_SubSector table(c);
        return table.select(table._sector_id() == _id());   
    }


    const Field<I_Sector>::Base& _id() const { 
        return _f_id._base; 
    }
    Field<I_Sector>::Base& _id() { 
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

    bool update(){
        std::map<std::string, std::string> field2Val;
        if (_f_id._dirty)
            field2Val["id"] = 
                toSQLString(_f_id._base);
        if (_f_name._dirty)
            field2Val["name"] = 
                toSQLString(_f_name._base);
        std::string where =
            "id=" + toSQLString(_f_id._base);
        if (DBFace::instance()->update("sectors", 
                                       field2Val, where))
        {
            _f_id._dirty = false;
            _f_name._dirty = false;
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

        
        if (DBFace::instance()->
                insert("sectors", field2Val,
                       _f_id._base.serialization()))
        {
            _f_id._dirty = false;
            _f_name._dirty = false;
            return true;
        }
        else
            return false;
                                   
    }
private:
    Field<I_Sector> _f_id;
    F_String _f_name;

    friend class T_Sector;
};

} //namespace DMMM
#endif //O_SECTOR
