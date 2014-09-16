#ifndef MAPCOUNTER_H
#define MAPCOUNTER_H

#include <iostream>

// This class holds elements as keys, with a counter (as a value) for each one. 
// when a new element is insterted, if it exists, it's counter (value) 
// is incremented. If it is a new element, then a new counter set to 1
// is created for 

// Note: uses class T operator==
// and T copy constructor
template<class T>
class MapCounter{
public:

    ~MapCounter(); 

    bool add(T* t);
    bool add(const T& t);
    vector<pair<T*,size_t>> get_top(size_t size);

    typename std::map<T*,size_t>::iterator begin(){ return mMap.begin(); }
    typename std::map<T*,size_t>::iterator end(){ return mMap.end(); }

private:
    std::map<T* const,size_t> mMap;
};


template<class T>
bool
MapCounter<T>::add(T* t){
    for( pair<T*,size_t> e : mMap)
        if( *(e.first) == *t){
            mMap[e.first]++;
            return true;
        }
    mMap[copy] = 1;
    return false;
}

template<class T>
bool
MapCounter<T>::add(const T& t){
    for( pair<T*,size_t> e : mMap)
        if( *(e.first) == t){
            mMap[e.first]++;
            return true;
        }
    T* copy = new T(t);
    mMap[copy] = 1;
    return false;    
}

template<class T>
vector<pair<T*,size_t>>
MapCounter<T>::get_top(size_t top_size){
    
    vector<pair<T*,size_t>> sorted;
    for (auto it = mMap.begin(); it != mMap.end(); ++it)
        sorted.push_back(*it);

    struct {
        bool operator()(pair<T*,size_t> a, pair<T*,size_t> b){
            return a.second > b.second;
        }
    }comp_func;
    std::sort(sorted.begin(), sorted.end(), comp_func);

    top_size = std::min(sorted.size(),top_size);
    vector<pair<T*,size_t>> ret(top_size);

    std::copy(sorted.begin(), sorted.begin()+top_size, ret.begin());

    return ret;
}


template<class T>
MapCounter<T>::~MapCounter(){
    while (mMap.size()){
        auto it = mMap.begin();
        free (it->first);
        mMap.erase(it);
    }
}

#endif //MAPCOUNTER_H
