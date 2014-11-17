#ifndef	_DMMM_ID_HPP
#define	_DMMM_ID_HPP

#include <iosfwd>
#include <string>
#include <sstream>

namespace DMMM{
namespace ID{

template<class T>
class Id {

public:
    explicit Id(size_t id) : _id(id) {}
    Id() {}

    inline Id operator++() { 
	++_id; return *this; 
    }
    
    inline Id operator++(int) { 
	Id val=*this; 
	++*this; 
	return val; 
    }

    inline bool operator==(const Id& that) const { 
	return _id == that._id; 
    }
    inline bool operator< (const Id& that) const { 
	return _id <  that._id; 
    }
    inline bool operator<=(const Id& that) const { 
	return _id <= that._id; 
    }
    inline bool operator> (const Id& that) const { 
	return _id > that._id; 
    }
    inline bool operator>= (const Id& that) const { 
	return _id >= that._id; 
    }
    inline bool operator!=(const Id& that) const { 
	return ! (*this == that); 
    }

    std::string to_s() const {
        std::ostringstream oss;
	oss << _id;
        return oss.str();
    }

    operator int() const{
        return _id;
    }

    inline size_t& serialization() { return _id; }
    inline const size_t& serialization() const { return _id; }

private:
    size_t _id;

};


} //namespace ID
} //namespace DMMM

template<class T>
std::ostream& 
operator<< (std::ostream& os, const DMMM::ID::Id<T>& id)
{ 
    os << id.to_s(); 
    return os;
}

template<class T>
std::istream& 
operator>> (std::istream& is, DMMM::ID::Id<T>& id)
{ 
    is >> id.serialization(); 
    return is;
}

#endif //_DMMM_ID_HPP
