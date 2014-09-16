#ifndef INSERTERS_HPP
#define INSERTERS_HPP

#include <vector>
#include <boost/thread/mutex.hpp>
#include "T_PageLink.hpp"
#include "T_PageImage.hpp"
#include "T_PageEmphasis.hpp"
#include "T_PageHeader.hpp"
#include "T_PageKeyword.hpp"
#include "WebPage.h"

template<class Table, class Obj>
class Inserter{
public:
    Inserter(size_t maxSize) : _maxSize(maxSize) {}
    Inserter() : _maxSize(5000) {}
    ~Inserter(){
        passToDB();
    }

    void add(const Obj& obj){
        _objects.push_back(obj);
        if (_objects.size() > _maxSize)
            passToDB();
    } 
    void addAndKeep(const Obj& obj){
        _objects.push_back(obj);
    } 

    void moveFrom(Inserter<Table, Obj>& inserter){     
        add(inserter._objects.begin(), inserter._objects.end());
        inserter._objects.clear();
    } 
    
    void add(typename std::vector<Obj>::const_iterator begin, 
             typename std::vector<Obj>::const_iterator end)
    {
        _objects.insert(_objects.end(), begin, end);
        if (_objects.size() > _maxSize)
            passToDB();
    }

    void passToDB(){
        _table.insertDirtyFields(_objects.begin(), _objects.end());
        _objects.clear();
    }
private:
    size_t                _maxSize;
    Table                 _table;
    std::vector<Obj>      _objects;
};

class DBInserters{

public:
    DBInserters() {}
    DBInserters(size_t insertersMaxSize) 
        : _linksInserter(insertersMaxSize),
          _imagesInserter(insertersMaxSize),
          _emphsInserter(insertersMaxSize),
          _headersInserter(insertersMaxSize),
          _keywordsInserter(insertersMaxSize)
        {}

    void add(const WebPage& wp){
        boost::mutex::scoped_lock lock(_mutex);
        _linksInserter.add(wp.mLinks.begin(), wp.mLinks.end());
	_headersInserter.add(wp.mHeaders.begin(), wp.mHeaders.end());
	_imagesInserter.add(wp.mImages.begin(), wp.mImages.end());
        _emphsInserter.add(wp.mEmphases.begin(), wp.mEmphases.end());
	_keywordsInserter.add(wp.mKeywords.begin(), wp.mKeywords.end());
    }
    
private:
    Inserter<DMMM::T_PageLink, DMMM::O_PageLink>             _linksInserter;
    Inserter<DMMM::T_PageImage, DMMM::O_PageImage>           _imagesInserter;
    Inserter<DMMM::T_PageEmphasis, DMMM::O_PageEmphasis>     _emphsInserter;
    Inserter<DMMM::T_PageHeader, DMMM::O_PageHeader> _headersInserter;
    Inserter<DMMM::T_PageKeyword, DMMM::O_PageKeyword>       _keywordsInserter;
    boost::mutex                                     _mutex;

};

#endif //INSERTERS_HPP
