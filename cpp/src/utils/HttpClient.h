#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <map>
#include <string>
#include <boost/filesystem.hpp>   
#include <curl/curl.h>
#include <boost/thread/mutex.hpp>
#include "Url.h"


class HttpClient{
public:
    HttpClient(const std::string& userAgent);
    ~HttpClient();

    void setTimeout(const size_t secs) { mTimeoutSecs = secs; }

    bool httpGet(const std::string& url, std::string& rPage)
    {
        return httpGet(url, "text/html", rPage);
    }
    bool httpGet(const std::string& url, 
                 const std::string& contentType,
                 std::string& rPage);
    bool httpPost(const std::string& url,
                 const std::string& contentType, std::string& postData,
                 std::string& rPage);
    bool httpGetWithRedirects(const Url& url,
                              const std::string& contentType,
                              std::string& rPage,
                              size_t maxRedirects);
    bool httpGetWithRedirects(const Url& url,
                              std::string& rPage,
                              size_t maxRedirects)
    {
        return httpGetWithRedirects(url, "text/html", rPage, maxRedirects);
    }
    size_t numRedirected() const { return mNumRedirected; }
    std::string lastUrl() const { return mLastUrl; }

    size_t getStatusCode() const { return mStatusCode; }
    CURLcode getHeaderStatusCode() const { return mHeaderErrorCode; }
    bool isStatusRedirect() const
        {
            return mStatusCode == 301 || mStatusCode == 302 
                || mStatusCode == 303 || mStatusCode == 307;
        }
    bool isWrongType() const { return mWrongType; }
    std::string getHeaderParam(const std::string& param) const;
    std::string domainIP() const;

private:
    bool processHeader(const std::string& header);

private:
    CURLcode                                   mHeaderErrorCode;
    size_t                                     mStatusCode;
    std::map<std::string, std::string>         mHeader;
    size_t                                     mTotalDownloaded;
    size_t                                     mTimeoutSecs;
    bool                                       mWrongType;
    std::string                                mUserAgent;
    std::string                                mLastUrl;
    size_t                                     mNumRedirected;

    CURL*                                      mCurlHandle;
    static bool                                mCurlGlobalInit;
    static boost::mutex                        mCurlGlobalInitMutex;
    
};

#endif //HTTPCLIENT_H
