// -*- c++ -*-
/*
 * identifier: a numerically-based ID that cannot be converted to a
 * number
 *
 * identifier<> is a template that lets you create new types of
 * identifiers.  Identifiers are *exactly* like unsigned ints, except
 * that they cannot be assigned to other unsigned ints or identifiers
 * of different types.  So, after
 *
 *    typedef identifier<Token> TokenId;
 *    typedef identifier<ContinuesMessage> MessageId;
 *    TokenId tid;
 *    MessageId mid;
 *    unsigned int id;
 *
 * all of the following are OK:
 *
 *    tid = TokenId(0);
 *    ++tid;
 *    mid = MessageId(100);
 *    ++mid;
 *
 * but none of the following will even compile:
 *
 *    tid = 11;
 *    mid = tid;
 *    tid = id;
 *    id = mid;
 *    id = tid+mid;
 *
 * The template parameter for identifier<> should be a unique class,
 * that is used to create the identifier type.  It is *not* used
 * anywhere in the class, so it doesn't matter which type you choose.
 * You should either choose a relevant type (like MessageId above) or
 * use an empty (and otherwise unused) "tag struct":
 *
 *    struct AtomTag {};
 *    typedef template<AtomTag> AtomId;
 *
 *
 * The 3rd parameter (AllowConversionToUInt) is for debugging ONLY.
 * If set to boost::mpl::true_ then a conversion to UInt will be
 * defined (however gcc will warn about all its uses).
 *
 * NOTE: Due to a bug in old versions of Boost serialization, you
 * *must* #include *_[io]archive.hpp before this file if you want to
 * serialize anything.
 */


#ifndef	_IDENTIFIER_H
#define	_IDENTIFIER_H

#include <boost/mpl/assert.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <iosfwd>

#include <string>
#include <sstream>

template<typename Container, typename Id> class container_id_wrapper;

template<class T, char dbgValChar = 0, typename UInt = unsigned int,
         typename AllowConversionToUInt = boost::mpl::false_>
class Identifier {
protected:
    UInt id_;
public:
    typedef UInt BaseType;

    explicit Identifier(UInt id) : id_(id) {}
    Identifier() {}				// Undefined id!

    // Default copy constructor, assignment, destructor are good

    inline Identifier operator++() { ++id_; return *this; }
    inline Identifier operator++(int) { Identifier val=*this; ++*this; return val; }

    inline bool operator==(const Identifier& that) const { return id_ == that.id_; }
    inline bool operator< (const Identifier& that) const { return id_ <  that.id_; }
    inline bool operator<=(const Identifier& that) const { return id_ <= that.id_; }
    inline bool operator> (const Identifier& that) const { return id_ > that.id_; }
    inline bool operator!=(const Identifier& that) const { return ! (*this == that); }

    inline void print(std::ostream& out)  const { out << id_; }

    // This function should be used *only* in debug messages (but
    // there is no C++ way to enforce this)
    std::string dbgval() const {
        std::ostringstream oss;
        if (dbgValChar)
            oss << dbgValChar;
        oss << id_;
        return oss.str();
    }


    // This is to be used for serialization only!
    inline UInt& valForSerialization() { return id_; }
    inline UInt valForSerialization() const { return id_; }

    // template<typename Container> friend class
//     container_id_wrapper<Container,Identifier>;

    // NOTE: gcc prints weird deprecation warnings here about "operator 9".  This is a gcc bug -- see http://gcc.gnu.org/bugzilla/show_bug.cgi?id=17947
    operator UInt() const __attribute__((deprecated)) {
        BOOST_MPL_ASSERT((AllowConversionToUInt));
        return id_;
    }

private:                        // boost serialize
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive& ar, unsigned int /* file version */) {
        ar & BOOST_SERIALIZATION_NVP(id_);
    }

};

#endif
