#ifndef UTILS_HPP
#define UTILS_HPP

#include <math.h>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <boost/filesystem.hpp>
#include "Identifier.h"

using namespace std;

template<class I>
void
deleteRange(I begin, I end)
{
    for(; begin != end; ++begin)
        delete *begin;
}

template<class I>
class IteratorRange : public std::pair<I, I> {
public:
    IteratorRange(const std::pair<I, I>& p)
        : std::pair<I, I>(p)
        {}
    IteratorRange(const I& i1, const I& i2)
        : std::pair<I, I>(i1, i2)
        {}
    IteratorRange() 
        {}
};

template<class I>
class IteratorSequence : public std::vector<IteratorRange<I> > {
};

template<class I>
void
intersect(const IteratorSequence<I>& s1, const IteratorSequence<I>& s2,
          IteratorSequence<I>& rs)
{
    size_t i = 0;
    size_t j = 0;
    while (i < s1.size() && j < s2.size()){
        I begin = max(s1[i].first, s2[j].first);
        I end = min(s1[i].second, s2[j].second);
        if (begin < end)
            rs.push_back(IteratorRange<I>(begin, end));
        if (s1[i].second < s2[j].second)
            ++i;
        else
            ++j;
    }
}

template<class T>
T
fromString(const std::string& s)
{
    std::istringstream iss;
    iss.str(s);
    
    T ret;
    iss >> ret;
    
    return ret;
}

template<class T>
std::string
toString(const T& t)
{
    std::ostringstream oss;
    oss << t;
    
    return oss.str();
}

template<class B>
size_t
countChar(const std::basic_string<B>& s, const B& c)
{
    size_t ret = 0;
    for (size_t i = 0; i < s.size(); ++i)
        if (s[i] == c)
            ++ret;
    return ret;
}

template<class B>
void
splitString(const std::basic_string<B>& s, 
            const B& seperator,
            std::vector<std::basic_string<B> >& rBits)
{
    rBits.clear();
    
    for (size_t i = 0; i < s.length();)
    {
        size_t j = s.find(seperator, i);
        if (j > i)
            rBits.push_back(s.substr(i, j - i));
        if (j == std::string::npos)
            break;
        i = j + 1;
    }
}

template<class B>
std::vector<std::basic_string<B> >
splitString(const std::basic_string<B>& s, 
            const B& seperator)
{
    std::vector<std::basic_string<B> > rBits;
    
    for (size_t i = 0; i < s.length();)
    {
        size_t j = s.find(seperator, i);
        if (j > i)
            rBits.push_back(s.substr(i, j - i));
        if (j == std::string::npos)
            break;
        i = j + 1;
    }
    return rBits;
}

template<class B>
void
splitString(const std::basic_string<B>& s, 
            const std::basic_string<B>& seperators,
            std::vector<std::basic_string<B> >& rBits)
{
    rBits.clear();
    
    for (size_t i = 0; i < s.length();)
    {
        size_t j = s.find_first_of(seperators, i);
        if (j > i)
            rBits.push_back(s.substr(i, j - i));
        if (j == std::string::npos)
            break;
        i = j + 1;
    }
}

template<class Ret, class It>
Ret
perlHash(It begin, It end, Ret init = 0)
{
    Ret id = init;
    for (; begin != end; ++begin)
	id = (id + (Ret) *begin) * 33;
    return id;
}

template<class Ret, class T>
Ret
perlHash(const T& t, Ret lastHash = 0)
{
    return (lastHash + (Ret)t) * 33;
}

class DummyHash32;
typedef Identifier<DummyHash32, 'H', int> Hash32;
template<class It>
Hash32
hash32(It begin, It end)
{
    return Hash32(perlHash<Hash32::BaseType, It>(begin, end));
}

class DummyHash64;
typedef Identifier<DummyHash64, 'H', unsigned long long> Hash64;
template<class It>
Hash64
hash64(It begin, It end)
{
    return Hash64(perlHash<Hash64::BaseType, It>(begin, end));
}

class DummyHash64;
typedef Identifier<DummyHash64, 'H', unsigned long long> Hash64;
template<class T>
Hash64
hash64(const std::vector<T>& v)
{
    Hash64::BaseType ret = 0;
    for (size_t i = 0; i < v.size(); ++i)
        ret = perlHash<Hash64::BaseType, 
                       typename T::const_iterator>
            (v[i].begin(), v[i].end(), ret);
    return Hash64(ret);
}

template<class T>
Hash64
hash64(const T& t, Hash64 lastHash = 0)
{
    return 
        Hash64(perlHash<Hash64::BaseType, T>(t, 
                                             lastHash.valForSerialization()));
}


template<class T, class S>
void
extractValues(const std::map<S, T>& m, std::vector<T>& vals)
{
    for (typename std::map<S, T>::const_iterator it = m.begin(); it != m.end();
	 ++it)
    {
	vals.push_back(it->second);
    }
}

template<class K, class V>
double
klDivergence(const std::map<K, V>& prior, const std::map<K, V>& post)
{
    double ret = 0.0;
    for (typename std::map<K, V>::const_iterator it = prior.begin();
	 it != prior.end(); ++it)
    {
	const K& key = it->first;
	double val = (double)it->second;
	typename std::map<K, V>::const_iterator itPost = post.find(key);
	if (itPost != post.end()){
	    double valPost = (double)(itPost->second);
	    ret +=  valPost * log(valPost / val);
	}
    }
    return ret;
}

template<class T>
std::map<T, size_t>
histogram(const std::vector<T>& v)
{
    std::map<T, size_t> ret;
    for (auto it = v.begin(); it != v.end(); ++it)
        ret[*it] += 1;
    return ret;
}

template<class T>
std::map<T, size_t>
operator + (const std::map<T, size_t>& h1, const std::map<T, size_t>& h2)
{
    std::map<T, size_t> ret = h1;
    for (auto it = h2.begin(); it != h2.end(); ++it)
        ret[it->first] += it->second;
    return ret;
}

template<class H>
void
addToHistogram(H& rH, const H& h)
{
    for (auto it = h.begin(); it != h.end(); ++it)
        rH[it->first] += it->second;
}

template<class T>
std::basic_string<T>
join(const std::vector<std::basic_string<T> >& v, const T& c)
{
    typename std::basic_string<T> ret;
    for (size_t i = 0; i < v.size(); ++i){
        if (i > 0)
            ret += c;
        ret += v[i];
    }
    return ret;
}

template<class T>
std::basic_string<T>
join(const std::vector<std::basic_string<T> >& v)
{
    typename std::basic_string<T> ret;
    for (size_t i = 0; i < v.size(); ++i)
        ret += v[i];
    return ret;
}

// file writing methods
void write_to_disk(string& content, string& info, const boost::filesystem::path& writeDest);

std::string firstWord(const std::string& s);

std::string substitute(const std::string& orig, const std::string& cut, 
                       const std::string& paste);
std::string trimSpaces(const std::string& s);

std::string toLower(const std::string& s);

std::string loadFileToString(const std::string& fileName);

boost::filesystem::path head(const boost::filesystem::path& p);
boost::filesystem::path decapitate(const boost::filesystem::path& p);

std::wstring cleanString(const std::string& stringToClean);

size_t getMemUsedMB();

#endif //UTILS_HPP
