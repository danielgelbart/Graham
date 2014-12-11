#ifndef DMMM_DBFACE_H
#define DMMM_DBFACE_H

#include <map>
#include <iosfwd>
#include <mysql++.h>
#include <boost/thread/mutex.hpp>
#include "dmmm_id.hpp"

namespace DMMM {

typedef std::vector<std::map<std::string, std::string> > QueryRes;

class DBFace{
public:
    static DBFace* instance() { return _this; }
    
    DBFace(const std::string& database, const std::string& host, 
	   const std::string& user, const std::string& password);
    DBFace(const std::string& database, const std::string& host, 
	   const std::string& user, const std::string& password,
	   std::ostream* os);

    ~DBFace() { _connection.disconnect(); }

    void connect();

    bool insert(const std::string& table, 
		const std::map<std::string, std::string>& field2Val,
		size_t& rInsertId);
    bool insert(const std::string& table, 
	        const std::vector<std::string>& fields,
	        const std::vector<std::vector<std::string> >& rows);


    bool update(const std::string& table, 
		const std::map<std::string, std::string>& field2Val,
		const std::string& where);
    bool select(const std::vector<std::string>& table, 
		const std::vector<std::string>& columns,
		const std::string& where,
		const std::string& additional,		
		QueryRes& rRes);
    bool erase(const std::vector<std::string>& table, 
	       const std::string& where);
    bool erase(const std::string& table, 
	       const std::string& where);
    size_t count(const std::vector<std::string>& tables, 
                 const std::string& where);

    std::string now(); 

    void startTransaction();
    void commitTransaction();

private:
    void log(const std::string& message);
    void logError(const std::string& message);
    bool getLastInsertId(mysqlpp::Query& rQuery, size_t& rId);
    bool executeQuery(mysqlpp::Query& rQuery);
    
    
private:
    mysqlpp::Connection                 _connection;
    mutable boost::mutex                _mutex;
    static DBFace*                      _this;		
    std::ostream*                       _os;
    std::string                         _host;
    std::string                         _user;
    std::string                         _database;
    std::string                         _password;
};




template<class T>
std::string
toSQLString(const ID::Id<T>& id)
{
    mysqlpp::Query q(0);
    q << mysqlpp::quote << id.to_s();	
    
    return q.str();
}

template<class T>
std::string
toSQLString(const T& t)
{
    mysqlpp::Query q(0);
    q << mysqlpp::quote << t;	
    
    return q.str();
}

std::string sanitize(const std::string& s);


} //namespace DMMM

#endif //DB_H
