#include <boost/date_time/posix_time/posix_time.hpp>
#include "dmmm_dbface.h"
#include "dmmm_utils.hpp"
using namespace mysqlpp;
using namespace std;

namespace DMMM {

DBFace* DBFace::_this = NULL;

DBFace::DBFace(const string& database, const string& host, 
	       const string& user, const string& password)
    : _connection(false), _os(NULL), _host(host), _user(user), _database(database),		
      _password(password)
{
    _this = this;
    connect();
}

DBFace::DBFace(const string& database, const string& host, 
               const string& user, const string& password,
               ostream* os)
    : _connection(false), _os(os), _host(host), _user(user), _database(database),		
      _password(password)
{
    _this = this;
    connect();
}

bool 
DBFace::switchDB(std::string& dbName)
{
    return _connection.select_db(dbName);
}

void
DBFace::connect()
{
    if (_connection.connected())
        _connection.disconnect();

    if(!_connection.set_option(new mysqlpp::ReconnectOption(true))) {
        cerr << "DB :: Can't set option to auto-reconnect";
        if (_os)
            (*_os) << "DB :: Can't set option to auto-reconnect";
    }
    if(!_connection.set_option(new mysqlpp::SetCharsetNameOption("utf8"))) {
        cerr << "DB :: Can't set CharsetNameOption to utf8";
        if (_os)
            (*_os) << "DB :: Can't set CharsetNameOption to utf8";
    }
    if (!_connection.connect(_database.c_str(), _host.c_str(), 
          _user.c_str(), _password.c_str())) {
        cerr << "DB :: Could not connect to DB";
        if (_os)
            (*_os) << "DB :: Could not connect to DB";
    }
}

string 
currentTime()
{
    using namespace boost::posix_time;
    ptime now = second_clock::local_time();

    return to_simple_string(now.time_of_day());
}


void
DBFace::log(const string& message)
{
    if (_os)
	(*_os) << currentTime() << " DMMM " << message << endl;
}

void
DBFace::logError(const string& message)
{
    log(string("ERROR: ") + message);
}

bool
DBFace::select(const vector<string>& tables, 
	       const vector<string>& columns,
	       const string& where, 
	       const string& additional, 
	       QueryRes& rRes)
{
    boost::mutex::scoped_lock lock(_mutex);

    Query q = _connection.query();
    
    q << "SELECT ";
    for (size_t i = 0; i < columns.size(); ++i){
	if (i > 0)
	    q << ",";
	q << tables[0] << "." << columns[i];
    }
    q << " FROM " << tables[0];
    for (size_t i = 1; i < tables.size(); ++i)
        q << ", " + tables[i];
    if (where.size())
	q << " WHERE " << where;
    if (additional.size())
	q << " " << additional;
    log(q.str());
    StoreQueryResult mysqlRes;
    try{
	mysqlRes = q.store();
    }
    catch (const mysqlpp::Exception& er) {
	logError(er.what());
	if (string(er.what()) == string("MySQL server has gone away")){
	    sleep(10);
	    connect();
	}
        return false;
    }
    
    for (size_t i = 0; i < mysqlRes.num_rows(); ++i){
	rRes.resize(rRes.size() + 1);
	for (size_t j = 0; j < columns.size(); ++j)
	    if (!mysqlRes[i][columns[j].c_str()].is_null())
 	        rRes.back()[columns[j]] = UTILS::toString(mysqlRes[i][columns[j].c_str()]);
    }
    return true;
}

size_t
DBFace::count(const vector<string>& tables, 
	      const string& where)
{
    boost::mutex::scoped_lock lock(_mutex);

    Query q = _connection.query();
    
    q << "SELECT count(*) as count FROM " << tables[0];
    for (size_t i = 1; i < tables.size(); ++i)
        q << ", " + tables[i];
    if (where.size())
	q << " WHERE " << where;
    log(q.str());
    StoreQueryResult mysqlRes;
    try{
	mysqlRes = q.store();
    }
    catch (const mysqlpp::Exception& er) {
	logError(er.what());
	if (string(er.what()) == string("MySQL server has gone away")){
	    connect();
	    sleep(10);
	}
        return false;
    }

    if (mysqlRes.num_rows() == 1)
	return UTILS::fromString<size_t>(UTILS::toString(mysqlRes[0]["count"]));
    else
        return 0;    
}


bool 
DBFace::erase(const vector<string>& tables, 
   	      const string& where)
{
    Query q = _connection.query();
    q << "DELETE FROM " << tables[0];
    if (tables.size() > 1)
        q << " USING " + tables[0] + " ";
    for (size_t i = 1; i < tables.size(); ++i)
        q << " INNER JOIN " + tables[i];
    if (where.size())
	q << " WHERE " << where;
    log(q.str());
    boost::mutex::scoped_lock lock(_mutex);
    return executeQuery(q);
    
}

bool 
DBFace::erase(const std::string& table, 
              const std::string& where)
{
    Query q = _connection.query();
    q << "DELETE FROM " << table;
    if (where.size())
	q << " WHERE " << where;
    log(q.str());
    boost::mutex::scoped_lock lock(_mutex);
    return executeQuery(q);
}
    

bool
DBFace::executeQuery(Query& q)
{
    try{
        q.execute();
    }
    catch (const mysqlpp::Exception& er) {
	logError(er.what());
	if (q.str() == "MySQL server has gone away"){
	    connect();
	    sleep(10);
	}
        return false;
    }
    return true;
}

bool
DBFace::getLastInsertId(Query& rQuery, size_t& rId)
{
    try{
        rId = rQuery.insert_id();
    }
    catch (const mysqlpp::Exception& er) {
	logError(er.what());
        return false;
    }
    return true;
}

bool 
DBFace::insert(const string& table, 
	       const vector<string>& fields,
	       const vector<vector<string> >& rows)
{
    if (fields.size() == 0 || rows.size() == 0)
	return true;

    Query q = _connection.query();
    q << "INSERT INTO " << table << " (";
    for (size_t i = 0; i < fields.size(); ++i){
	if (i != 0)
	    q << ",";
	q << fields[i];
    }
    q << ")VALUES";
    for (size_t i = 0; i < rows.size(); ++i){
	if (i != 0)
	    q << ",";
	q << "(";
	for(size_t j = 0; j < rows[i].size(); ++j){
	    if (j != 0)
		q << ",";
	    q << rows[i][j];
	}
	q << ")";
    }
    log(q.str());
    boost::mutex::scoped_lock lock(_mutex);
    bool ok = executeQuery(q);
    return ok;
}


bool 
DBFace::insert(const string& table, 
	       const map<string, string>& field2Val,
	       size_t& rInsertId)
{
    if (field2Val.size() == 0)
	return true;

    Query q = _connection.query();
    q << "INSERT INTO " << table << " (";
    for (map<string, string>::const_iterator it = field2Val.begin(); 
	 it != field2Val.end(); ++it)
    {
	if (it != field2Val.begin())
	    q << ",";
	q << it->first;
    }
    q << ")VALUES(";
    for (map<string, string>::const_iterator it = field2Val.begin(); 
	 it != field2Val.end(); ++it)
    {
	if (it != field2Val.begin())
	    q << ",";
	q << it->second;
    }
    q << ")";
    log(q.str());
    boost::mutex::scoped_lock lock(_mutex);
    bool ok = executeQuery(q);
    ok = ok && getLastInsertId(q, rInsertId);
    return ok;
}

bool
DBFace::update(const string& table, 
	       const map<string, string>& field2Val,
	       const string& where)
{
    Query q = _connection.query();
    
    q << "UPDATE " << table << " SET ";
    for (map<string, string>::const_iterator it = field2Val.begin();
	 it != field2Val.end(); ++it)
    {
	if (it != field2Val.begin())
	    q << ",";
	q << it->first << "=" << it->second;
    }
    if (where.size())
	q << " WHERE " << where;
    log(q.str());
    boost::mutex::scoped_lock lock(_mutex);
    return executeQuery(q);
}

string 
DBFace::now()
{
    boost::mutex::scoped_lock lock(_mutex);
    Query q = _connection.query();
    
    q << "SELECT NOW()";
    StoreQueryResult mysqlRes;
    try{
	mysqlRes = q.store();
    }
    catch (const mysqlpp::Exception& er) {
        cerr << "Query failed: " << q << endl << er.what();
        return string();
    }
    return UTILS::toString(mysqlRes[0]["now()"]);
}

string 
DBFace::getDBName()
{
    boost::mutex::scoped_lock lock(_mutex);
    Query q = _connection.query();
    q << "SELECT DATABASE()";
    log(q.str());
    StoreQueryResult mysqlRes;
    try{
	mysqlRes = q.store();
    }
    catch (const mysqlpp::Exception& er) {
        cerr << "Query failed: " << q << endl << er.what();
        return string();
    }
    return UTILS::toString(mysqlRes[0]["database()"]);
}


string
sanitize(const string& s)
{
    mysqlpp::Query q(0);
    string escaped;
    q.escape_string(&escaped, s.c_str());
    q << escaped;	
    
    return q.str();
}

void 
DBFace::startTransaction()
{
    Query q = _connection.query();
    
    q << "START TRANSACTION";
    boost::mutex::scoped_lock lock(_mutex);
    executeQuery(q);
}

void 
DBFace::commitTransaction()
{
    Query q = _connection.query();
    
    q << "COMMIT";
    boost::mutex::scoped_lock lock(_mutex);
    executeQuery(q);
}

} //namespace DMMM
