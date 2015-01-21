#ifndef O_NOTE
#define O_NOTE

#include <map>
#include <string>
#include "dmmm_utils.hpp"
#include "dmmm_dbface.h"
#include "dmmm_identifiers.hpp"
#include "dmmm_fields.hpp"
#include "dmmm_comparators.hpp"

enum EnumNoteCLASS { K10 = 1, Q10 = 2, REPORTS = 3, INCOME_REPORT = 4, BALANCE_REPORT = 5, COVER_REPORT = 6 };


namespace DMMM {

class O_Note{
public:

    O_Note() {}
    O_Note(const I_Stock& parent_id) 
        : _f_stock_id(parent_id)
        {}

O_Note(const I_Note& id) 
        : _f_id(id)
        {}


    const Field<I_Note>::Base& _id() const { 
        return _f_id._base; 
    }
    Field<I_Note>::Base& _id() { 
        _f_id._dirty = true; 
        return _f_id._base; 
    }
    const F_Fixnum::Base& _stock_id() const { 
        return _f_stock_id._base; 
    }
    F_Fixnum::Base& _stock_id() { 
        _f_stock_id._dirty = true; 
        return _f_stock_id._base; 
    }
    const F_Fixnum::Base& _year() const { 
        return _f_year._base; 
    }
    F_Fixnum::Base& _year() { 
        _f_year._dirty = true; 
        return _f_year._base; 
    }
    const Field<EnumNoteCLASS>::Base& _class() const { 
        return _f_class._base; 
    }
    Field<EnumNoteCLASS>::Base& _class() { 
        _f_class._dirty = true; 
        return _f_class._base; 
    }
    const F_String::Base& _note() const { 
        return _f_note._base; 
    }
    F_String::Base& _note() { 
        _f_note._dirty = true; 
        return _f_note._base; 
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
        if (_f_stock_id._dirty)
            field2Val["stock_id"] = 
                toSQLString(_f_stock_id._base);
        if (_f_year._dirty)
            field2Val["year"] = 
                toSQLString(_f_year._base);
        if (_f_class._dirty)
            field2Val["class"] = 
                toSQLString(_f_class._base);
        if (_f_note._dirty)
            field2Val["note"] = 
                toSQLString(_f_note._base);
        if (_f_created_at._dirty)
            field2Val["created_at"] = 
                toSQLString(_f_created_at._base);

        field2Val["updated_at"] = "NOW()";

        std::string where =
            "id=" + toSQLString(_f_id._base);
        if (DBFace::instance()->update("notes", 
                                       field2Val, where))
        {
            _f_id._dirty = false;
            _f_stock_id._dirty = false;
            _f_year._dirty = false;
            _f_class._dirty = false;
            _f_note._dirty = false;
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
        if (_f_stock_id._dirty)
            field2Val["stock_id"] = 
                toSQLString(_f_stock_id._base);
        if (_f_year._dirty)
            field2Val["year"] = 
                toSQLString(_f_year._base);
        if (_f_class._dirty)
            field2Val["class"] = 
                toSQLString(_f_class._base);
        if (_f_note._dirty)
            field2Val["note"] = 
                toSQLString(_f_note._base);

        field2Val["created_at"] = "NOW()";

        if (_f_updated_at._dirty)
            field2Val["updated_at"] = 
                toSQLString(_f_updated_at._base);

        
        if (DBFace::instance()->
                insert("notes", field2Val,
                       _f_id._base.serialization()))
        {
            _f_id._dirty = false;
            _f_stock_id._dirty = false;
            _f_year._dirty = false;
            _f_class._dirty = false;
            _f_note._dirty = false;
            _f_created_at._dirty = false;
            _f_updated_at._dirty = false;
            return true;
        }
        else
            return false;
                                   
    }
private:
    Field<I_Note> _f_id;
    F_Fixnum _f_stock_id;
    F_Fixnum _f_year;
    Field<EnumNoteCLASS> _f_class;
    F_String _f_note;
    F_Time _f_created_at;
    F_Time _f_updated_at;

    friend class T_Note;
};

} //namespace DMMM
#endif //O_NOTE
