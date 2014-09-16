#ifndef LOGGER_H
#define LOGGER_H

#include <map>
#include <string>
#include <iosfwd>
#include <boost/thread/mutex.hpp>
#include "Utils.hpp"

// This is the interface to the program log. Sample usage:
// LOG(LL_WARNING, "all birds have flown away")
// These messages are written to the file named in the
// configuration parameter system.log_file_name

enum LogLevel { LL_ERROR = 0,
                LL_WARNING = 1,
                LL_INFO = 2,
                LL_DEBUG = 3,
                LL_TRACE = 4 };


class Logger{
private:
    static Logger* mThis;

public:
    static Logger* instance() { return mThis; }

    Logger(const char* logFileName);
    ~Logger();

    //check if log message should be written to log
    bool logable(LogLevel level, const char* file, const char* function);

    //write log info
    void logInfo(LogLevel level, const char* file, const char* function);
    //write log message
    void logMessage(const std::string& entry);

    size_t numErrors() { return mNumMessages[LL_ERROR]; }
    size_t numWarning() { return mNumMessages[LL_WARNING]; }

    std::ofstream* logFile() const { return mLogFile; }

private:
    LogLevel                      mMaxLevel;
    std::ofstream*                mLogFile;
    std::map<LogLevel, size_t>    mNumMessages;
    boost::mutex                  mMutex;
    std::string                   mLogFileName;
    bool                          mEmailed;
};


class LogMessage{
public:
    LogMessage(LogLevel level, const char* file, const char* function)
        : mLogLevel(level), mFile(file), mFunction(function), mHeader(true)
        {
            mLogable = Logger::instance()->logable(level, file, function);
        }

    template<class T>
    LogMessage&
    operator << (const T& t) {
        if (mLogable){
            if (mHeader){
                Logger::instance()->logInfo(mLogLevel, mFile, mFunction);
                mHeader = false;
            }
            Logger::instance()->logMessage(toString(t));
        }  
        return *this;
    }

private:
    LogLevel                      mLogLevel;
    const char*                   mFile;
    const char*                   mFunction;
    bool                          mLogable;
    bool                          mHeader;
};


#define	LOG(level)  LogMessage(level,     __FILE__, __FUNCTION__)

#define LOG_ERROR   LogMessage(LL_ERROR,   __FILE__, __FUNCTION__)
#define LOG_WARNING LogMessage(LL_WARNING, __FILE__, __FUNCTION__)
#define LOG_INFO    LogMessage(LL_INFO,    __FILE__, __FUNCTION__)
#define LOG_DEBUG   LogMessage(LL_DEBUG,   __FILE__, __FUNCTION__)
#define LOG_TRACE   LogMessage(LL_TRACE,   __FILE__, __FUNCTION__)

#endif //LOGGER_H

