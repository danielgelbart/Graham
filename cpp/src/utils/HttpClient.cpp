#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/timer.hpp>
#include <boost/filesystem.hpp>   
#include <boost/filesystem/fstream.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include "Logger.h"
#include "Config.h"
#include "HttpClient.h"
#include "Url.h"

using boost::asio::ip::tcp;
using namespace boost::filesystem;
using namespace std;
using boost::system::error_code;
namespace nsBFS = boost::filesystem;


bool HttpClient::mCurlGlobalInit = false;
boost::mutex HttpClient::mCurlGlobalInitMutex;

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t
WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)data;

    mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) {
        /* out of memory! */ 
        printf("not enough memory (realloc returned NULL)\n");
        exit(EXIT_FAILURE);
    }

    memcpy(&(mem->memory[mem->size]), ptr, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

HttpClient::HttpClient(const string& userAgent) 
    : mStatusCode(0),
      mTotalDownloaded(0),
      mWrongType(false),
      mUserAgent(userAgent),
      mCurlHandle(0)
{
    LOG_INFO << "Initialized HttpClient with user agent " << mUserAgent;
    mTimeoutSecs = confParam<size_t>("get_timeout_secs");
    {
        boost::mutex::scoped_lock lock(mCurlGlobalInitMutex);
    
        if (!mCurlGlobalInit){
            curl_global_init(CURL_GLOBAL_SSL);
            mCurlGlobalInit = true;
        }
    }
    mCurlHandle = curl_easy_init();
    if (mCurlHandle == 0)
        LOG_ERROR << "Could not get curl handle";
    else
        LOG_INFO << "Got curl handle " << mCurlHandle;

    curl_easy_setopt(mCurlHandle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(mCurlHandle, CURLOPT_HEADERFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(mCurlHandle, CURLOPT_TIMEOUT, mTimeoutSecs);
    curl_easy_setopt(mCurlHandle, CURLOPT_CONNECTTIMEOUT, mTimeoutSecs);
    curl_easy_setopt(mCurlHandle, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(mCurlHandle, CURLOPT_MAXFILESIZE, 5000000);

    string interfaces = 
	haveParam("httpclient.interfaces") ?
	confParam<string>("httpclient.interfaces") :
	"";
    if (interfaces.size()){
	vector<string> faces;
	splitString(interfaces, ',', faces);
	string face = trimSpaces(faces[rand() % faces.size()]);
	curl_easy_setopt(mCurlHandle, CURLOPT_INTERFACE, face.c_str());
        LOG_INFO << "Using interface " << face;
    }
    else
        LOG_INFO << "Using default interface";
}

HttpClient::~HttpClient()
{
    LOG_INFO << "shutting down httpclient";
    curl_easy_cleanup(mCurlHandle); 
}

string
toHumanReadable(size_t i)
{
    string ret;
    if (i < 1024 * 10)
        ret = toString(i) + "B";
    else if(i < 1024 * 1024 * 10)
        ret = toString(i / 1024) + "KB";
    else
        ret = toString(i / 1024 / 1024) + "MB";

    return ret;
}

string 
escapeUrl(const string& s)
{
    string ret;
    for (size_t i = 0; i < s.size(); ++i)
        switch(s[i]){
            case '<':
                ret += "%3C";
                break;
            case '>':
                ret += "%3E";
                break;
//            case '%':   
//                ret += "%25";
//                break;
            case '{':   
                ret += "%7B";
                break;
            case '}':   
                ret += "%7D";
                break;
            case '|':
                ret += "%7C";
                break;
            case '\'': 
                ret += "%5C";
                break;
            case '^':   
                ret += "%5E";
                break;
            case '[':
                ret += "%5B";
                break;
            case ']':   
                ret += "%5D";
                break;
            case '`':
                ret += "%60";
                break;
            case ';':
                ret += "%3B";
                break;
            case '@': 
                ret += "%40";
                break;
//            case '&':
//                ret += "&amp;";
//                break;
            case '$':   
                ret += "%24";
                break;
            case ' ':
                ret += "%20";
                break;
            default:
                ret += s[i];
        }
    
    LOG_INFO << "actual get string: " << ret;
    return ret;
}


bool
HttpClient::processHeader(const string& header)
{    
    vector<string> headerLines;
    splitString(header, string("\n"), headerLines);

    mHeader.clear();
    for (size_t j = 0; j < headerLines.size(); ++j){
        vector<string> split;
        splitString(headerLines[j], string(" \r"), split);
        if (split.size() >= 2){
            split[0] = toLower(split[0]);
            if (mHeader[split[0]].size())
                mHeader[split[0]] += '\n';
            for (size_t i = 1; i < split.size(); ++i)
                mHeader[split[0]] += split[i];
            LOG_DEBUG << "Header: " << split[0] << " " << mHeader[split[0]];
        }
    }
    return true;
}


// url - url to get
// contentType
// rPage - returned content
bool
HttpClient::httpGet(const string& url, const string& contentType, 
                    string& rPage)
{
    LOG_INFO << "GETing " << url;

    mLastUrl = url;

    curl_easy_setopt(mCurlHandle, CURLOPT_URL, escapeUrl(url).c_str());
    
    struct MemoryStruct chunk;
    chunk.memory = (char*)malloc(1); 
    chunk.size = 0;   
    struct MemoryStruct headerChunk;
    headerChunk.memory = (char*)malloc(1); 
    headerChunk.size = 0;   

    curl_easy_setopt(mCurlHandle, CURLOPT_ENCODING, "");
    curl_easy_setopt(mCurlHandle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(mCurlHandle, CURLOPT_HEADERDATA, (void *)&headerChunk);
    curl_easy_setopt(mCurlHandle, CURLOPT_FOLLOWLOCATION, 0);    
    curl_easy_setopt(mCurlHandle, CURLOPT_USERAGENT, mUserAgent.c_str());

    mHeaderErrorCode = curl_easy_perform(mCurlHandle);
    if (mHeaderErrorCode != CURLE_OK){
        LOG_INFO << "curl says \"" << curl_easy_strerror(mHeaderErrorCode) 
                 << '"';
        return false;
    }

    rPage = string(chunk.memory  ? chunk.memory : "");
    free(chunk.memory);
    string header = string(headerChunk.memory ? headerChunk.memory  : "");
    free(headerChunk.memory);
    
    string encoding;
    string utf8Body;
    
    long tempStatusCode;
    curl_easy_getinfo(mCurlHandle, CURLINFO_RESPONSE_CODE, &tempStatusCode);
    mStatusCode = tempStatusCode;
    if (!processHeader(header))
        return false;

    string type = toLower(getHeaderParam("content-type:")); 
    if (mStatusCode == 200 && 
        (type.size() < contentType.size() || 
         type.substr(0, contentType.size()) != contentType))
    {
        LOG_INFO << "Content type differs: Content-Type: " << type 
                 << " rather than " << contentType;
        mWrongType = true;
        //return false;
    }
    else
        mWrongType = false;

    return true;
}

bool
HttpClient::httpGetWithRedirects(const Url& url,
                                 const std::string& contentType,
                                 std::string& rPage,
                                 size_t maxRedirects)
{
    LOG_INFO << "GETing " << url.fullAddr() << " with max allowed redirects: "
             << maxRedirects;
    
    Url url1 = url;
    for (size_t i = 0; i < maxRedirects; ++i){
        mNumRedirected = i;
        if (!httpGet(url1.fullAddr(), contentType, rPage))
            return false;

        if (isStatusRedirect()){
            string redirectTo = getHeaderParam("location:"); 
            LOG_INFO << mStatusCode << " Redirect to: " << redirectTo;
            Url link;
            if (!link.setFromLink(url1, redirectTo)){
                LOG_WARNING << "Could not parse link: " << redirectTo;
                continue;
            }
            if (link.domain() != url1.domain()){
                LOG_INFO << "not following redirects outside domain";
                break;
            }
            if (link == url1){
                LOG_INFO << "Redirect loop - exiting";
                break;
            }
            url1 = link;
            rPage.clear();
        }
        else
            return true;
    }
    return false;
}

string 
HttpClient::getHeaderParam(const string& param) const
{
    auto it = mHeader.find(param);
    if (it != mHeader.end())
        return it->second;
    else
        return string();
}

bool
HttpClient::httpPost(const string& url, const string& contentType, 
                     string& postData, string& rPage)
{
    LOG_INFO << "GETing with HTTP POST " << url;

    mLastUrl = url;

    struct MemoryStruct chunk;
    chunk.memory = (char*)malloc(1);
    chunk.size = 0;
    struct MemoryStruct headerChunk;
    headerChunk.memory = (char*)malloc(1);
    headerChunk.size = 0;

    curl_easy_setopt(mCurlHandle, CURLOPT_ENCODING, "");
    curl_easy_setopt(mCurlHandle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(mCurlHandle, CURLOPT_HEADERDATA, (void *)&headerChunk);
    curl_easy_setopt(mCurlHandle, CURLOPT_FOLLOWLOCATION, 0);
    curl_easy_setopt(mCurlHandle, CURLOPT_USERAGENT, mUserAgent.c_str());

    curl_easy_setopt(mCurlHandle, CURLOPT_POSTFIELDS, postData.c_str());
    curl_easy_setopt(mCurlHandle, CURLOPT_URL, escapeUrl(url).c_str());

    CURLcode code =
        curl_easy_perform(mCurlHandle);
    LOG_INFO << "curl says \"" << curl_easy_strerror(code) << '"';

    rPage = string(chunk.memory  ? chunk.memory : "");
    free(chunk.memory);
    string header = string(headerChunk.memory ? headerChunk.memory  : "");
    free(headerChunk.memory);

    string encoding;
    string utf8Body;

    long tempStatusCode;
    curl_easy_getinfo(mCurlHandle, CURLINFO_RESPONSE_CODE, &tempStatusCode);
    mStatusCode = tempStatusCode;
    if (!processHeader(header))
        return false;

    string type = toLower(getHeaderParam("content-type:"));
    if (mStatusCode == 200 &&
        (type.size() < contentType.size() ||
         type.substr(0, contentType.size()) != contentType))
    {
        LOG_INFO << "Not saving: Content-Type: " << type
                 << " rather than " << contentType;
        mWrongType = true;
        return false;
    }
    else
        mWrongType = false;

    return true;
}

string 
HttpClient::domainIP() const
{
    char* c;
    curl_easy_getinfo(mCurlHandle, CURLINFO_PRIMARY_IP, &c);
    return string(c);
}

