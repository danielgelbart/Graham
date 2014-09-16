#ifndef PHRASER_H
#define PHRASER_H

#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <boost/unordered_map.hpp>
#include "UTFConverter.h"


void buildPhrases(const std::string& text0);

template<class T>
class Phraser {

public:
    void addSentence(const std::vector<T>& sen){
	if (sen.size() > 0)
	    ++_phrases[sen];
    }

    void addPhraser(const Phraser<T>& p){
        addToHistogram(_phrases, p._phrases);
    }

    std::map<std::vector<T>, size_t> 
    getPhrases(const size_t minFreq = 2) const;

    void printPhrases(std::ostream& os, const size_t maxLength) const;

    size_t size() const { return _phrases.size(); }
    
private:
    std::map<std::vector<T>, size_t >   _phrases;
};

template<class T>
std::map<typename std::vector<T>, size_t>
buildPhrases(const typename std::vector<std::vector<T> >& doc)
{
    Phraser<T> phraser;

    for (size_t i = 0; i < doc.size(); ++i)
	phraser.addSentence(doc[i]);

    return phraser.getPhrases(); 
}

std::map<std::vector<std::wstring>, size_t> 
buildFromText(const std::string& text, Phraser<std::wstring>& p);

std::string vecwToString(const std::vector<std::wstring>& vw);

template<class K, class V>
void
mapKeysToVector(const std::map<K, V>& m, std::vector<K>& r)
{
    for (typename std::map<K, V>::const_iterator it = m.begin();
	 it != m.end(); ++it)
    { 
	r.push_back(it->first);
    }
}

template<class V>
size_t
maxLength(const std::vector<V>& v)
{
    size_t maxLen = 0;
    for (size_t i = 0; i < v.size(); ++i)
	maxLen = std::max(maxLen, v[i].size());
    return maxLen;
}

template<class T>
std::vector<T>
subvec(const std::vector<T>& v, size_t i, size_t len)
{
    std::vector<T> ret(len);
    for (size_t k = 0; k < len; ++k)
	ret[k] = v[k + i];
    return ret;
}

template<class K>
void
removeBelowThreshold(std::map<K, size_t>& r, size_t thresh)
{
    typename std::map<K, size_t>::iterator itNext = r.begin();
    for(typename std::map<K, size_t>::iterator it = itNext; 
	    it != r.end(); it = itNext)
    {
	++itNext;
	if (it->second < thresh)
	    r.erase(it);
    }
}

template<class T>
void
removeSubphrases(std::map<size_t, std::map<std::vector<T>, size_t> >& app, 
		 const size_t len)
{
    for(typename std::map<std::vector<T>, size_t>::iterator
	    itA = app[len].begin(); itA != app[len].end(); ++itA)
    {
	std::vector<T> p = itA->first;
	for (size_t len0 = 1; len0 < len; ++len0){
	    for (size_t i0 = 0; i0 + len0 <= p.size(); ++i0){
		std::vector<T> sub = subvec(p, i0, len0);
		typename std::map<std::vector<T>, size_t>::iterator itP =
		    app[sub.size()].find(sub);
		if (itP != app[sub.size()].end() && itP->second == itA->second)
		    app[sub.size()].erase(sub);
	    }
	}
	
    }
}


bool allowed(const std::vector<std::wstring>& phrase);

template<class T> void removeNotAllowed(std::map<std::vector<T>, size_t>& r)
{
}

template<>
void 
removeNotAllowed<std::wstring>(std::map<std::vector<std::wstring>, size_t>& r);


template<class T>
std::map<std::vector<T>, size_t>
Phraser<T>::getPhrases(const size_t minFreq) const 
{
    std::map<std::vector<T>, size_t> ret;
    std::vector<std::vector<T> > pvec;
    mapKeysToVector(_phrases, pvec);
    size_t maxLen = maxLength(pvec);
    
    std::map<size_t, std::map<std::vector<T>, size_t> > appearances;

    for (size_t len = maxLen; len > 0; --len){
	for (size_t i = 0; i < pvec.size(); ++i){
	    const std::vector<T>& p = pvec[i];
	    
	    for (size_t i0 = 0; i0 + len <= p.size(); ++i0)
		++appearances[len][subvec(p, i0, len)];
	}
	
	removeBelowThreshold(appearances[len], minFreq);
	removeNotAllowed<T>(appearances[len]);
    }
    for (size_t len = maxLen; len > 0; --len)
	removeSubphrases(appearances, len);

    for (size_t len = maxLen; len > 0; --len){
	for (typename std::map<std::vector<T>, size_t>::const_iterator it =
		 appearances[len].begin(); it != appearances[len].end(); ++it)
	{
	    ret[it->first] = it->second;
	}
		 
    }
    return ret;
}


template<class T>
void 
Phraser<T>::printPhrases(std::ostream& os, const size_t maxLength) const
{
    std::map<std::vector<T>, size_t > phrases = getPhrases();
    for (typename std::map<std::vector<T>, size_t >::const_iterator it = 
	     phrases.begin(); it != phrases.end(); ++it)
    {
	if (it->first.size() > maxLength)
	    continue;
	os << it->second << "\t";
	for (size_t i = 0; i < it->first.size(); ++i)
	{
	    os << wstrToUtf8(it->first[i]) << " ";
	}
	os << std::endl;
    }
}


#endif //PHRASER_H
