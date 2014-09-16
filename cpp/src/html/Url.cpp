#include <iostream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include "Utils.hpp"
#include "Url.h"

using namespace std;




string
unescapeUrl(const string& s)
{
    char* temps = curl_unescape(substitute(trimSpaces(s),"&amp;", "&").c_str(), 0);
    string unescapedURL(temps);
    //unescapedURL = substitute(unescapedURL,"&amp;", "&");
    curl_free(temps);

    boost::replace_all( unescapedURL, "\r\n", " " );
    boost::replace_all( unescapedURL, "\r", " " );
    boost::replace_all( unescapedURL, "\n", " " );
    boost::replace_all( unescapedURL, "\t", " " );

    return trimSpaces(unescapedURL);
}

string
removedRedundancies(const string& path)
{ 
    vector<string> dirs;
    size_t pos = 0;
    while (pos < path.size()){
        size_t nextPos = path.find('/', pos);
        if (nextPos == string::npos)
            nextPos = path.size();
        else
            ++nextPos;
        string d = path.substr(pos, nextPos - pos);
        pos = nextPos;
        if (d == "./")
            continue;
        if (dirs.size() && d == "../"){
            dirs.pop_back();
            continue;
        }
        if (dirs.size() && d == "/")
            continue;
        dirs.push_back(d);
    }
    
    string ret;
    for (size_t i = 0; i < dirs.size(); ++i)
        ret += dirs[i];
    return ret;
}

bool
Url::isLegal() const
{
    string full = fullAddr();
    if (full.find("javascript:") != string::npos)
        return false;
    else if (full.find("mailto:") != string::npos)
        return false;
    else if (mLeaf.size() > 0 && mLeaf[0] == '#')
        return false;
    else return 
             boost::regex_match(mDomain, 
                                boost::regex("^((([a-zA-Z0-9]|-)+)\\.)+([a-zA-Z])+$"));
}

void
Url::regularizeDomain()
{
    if (mDomain.size() && *mDomain.rbegin() == '/')
        mDomain = mDomain.substr(0, mDomain.size() - 1);
    if (mDomain.size() >= 7 && mDomain.substr(0, 7) == "http://"){
        mProtocol = PAGE_PROTOCOL_HTTP;
        mDomain = mDomain.substr(7);
    }
    else if (mDomain.size() >= 8 && mDomain.substr(0, 8) == "https://"){
        mProtocol = PAGE_PROTOCOL_HTTPS;
        mDomain = mDomain.substr(8);
    }
    mDomain = toLower(mDomain);
}

void
Url::extractDomain(const string& url)
{
    if (url.length() >= 8 && url.substr(0, 7) == "http://"){
        size_t domainEndPos = url.find('/', 7);
        mDomain = domainEndPos == string::npos ? 
            url.substr(7): 
            url.substr(7, domainEndPos - 7);
        mProtocol = PAGE_PROTOCOL_HTTP;
    }
    else if (url.length() >= 9 && url.substr(0, 8) == "https://"){
        size_t domainEndPos = url.find('/', 8);
        mDomain = domainEndPos == string::npos ? 
            url.substr(8): 
            url.substr(8, domainEndPos - 8);
        mProtocol = PAGE_PROTOCOL_HTTPS;
    }
    regularizeDomain();
} 

bool
Url::setFromDomainAndLocal(const string& domain, const string& localUrl)
{
    string unescsapedURL = unescapeUrl(localUrl);

    mDomain = domain;
    regularizeDomain();
    if (mDomain.size() == 0 || unescsapedURL.size() == 0)
        return false;
    setFromLocalUrl(unescsapedURL);
    
    return isLegal();
}

bool
Url::setFromLink(const Url& url, const string& link)
{
    string unescsapedURL = unescapeUrl(link);

    mProtocol = url.mProtocol;
    if (unescsapedURL.size() == 0)
        return false; 
    if (unescsapedURL.find("http://") == 0)
        return setFromFullUrl(unescsapedURL);
    else if (unescsapedURL.find("https://") == 0)
        return setFromFullUrl(unescsapedURL);
    else if (unescsapedURL.length() > 2 && unescsapedURL.substr(0, 2) == "//"){
        if (url.protocol() == PAGE_PROTOCOL_HTTP)
            return setFromFullUrl("http:" + unescsapedURL);
        else
            return setFromFullUrl("https:" + unescsapedURL);
    }
    else{
        mDomain = url.mDomain;
        if (unescsapedURL[0] == '/')
            return setFromLocalUrl(unescsapedURL);
        else
            return setFromRelativeLink(url, unescsapedURL);
    }
}


bool
Url::setFromRelativeLink(const Url& url, const string& link)
{ 
    if (link[0] == '#'){
        *this = url;
        return true;
    }
    
    if (link[0] == '?'){
        *this = url;
	mParams = link;
	return true;
    }

    if (!setFromLocalUrl(link))
        return false;

    mPath = url.mPath + mPath;
    mPath = removedRedundancies(mPath);

    return isLegal();
}

bool
Url::setFromFullUrl(const string& fullUrl)
{
    string unescsapedURL = unescapeUrl(fullUrl);

    extractDomain(unescsapedURL);
    if (!mDomain.size())
        return false;
    size_t domainEndPos = toLower(unescsapedURL).find(mDomain) + mDomain.size();
    string page = unescsapedURL.substr(domainEndPos);
    if (page.size() == 0)
	page = "/";
    setFromLocalUrl(page);
    return isLegal();
}

void
Url::trimParams()
{
    string p = mParams;
    mParams.clear();
    size_t pos = 0;
    while (pos < p.size()){
        size_t nextPos = min(p.find_first_of(";&", pos), p.size() - 1);
        ++nextPos;
        if (p.substr(pos, 10) != string("jsessionid"))
            mParams += p.substr(pos, nextPos - pos);
        pos = nextPos;
    }
    if (mParams.size() && 
        ((mParams[mParams.size() - 1] == ';') || 
         (mParams[mParams.size() - 1] == '&')))
    {
        mParams = mParams.substr(0, mParams.size() - 1);
    }

}


bool
Url::setFromLocalUrl(const string& localUrl)
{
    size_t hashPos = min(localUrl.find('#'), localUrl.size());
    string local = unescapeUrl(localUrl.substr(0, hashPos)); 
    size_t pos = 0;
    while(pos < local.size()){
        size_t nextPos = local.find_first_of("/?;", pos);
        char c = (nextPos == string::npos) ? 0 : local[nextPos];
        switch (c){
        case 0:
            mLeaf = local.substr(pos);
            break;
        case '/':
            ++nextPos;
            mPath += local.substr(pos, nextPos - pos);
            break;
        case ';':
        case '?':
            mLeaf = local.substr(pos, nextPos - pos);
            if (nextPos < local.size() - 1)
                mParams = local.substr(nextPos);
            nextPos = local.size();
            break;
        }
        pos = nextPos;
    }
    trimParams();
    return isLegal();
}

string 
Url::fullAddr() const 
{ 
	string url = trimSpaces(toLower(mDomain) + localAddr());
	if (!url.empty())
		url = mProtocol == PAGE_PROTOCOL_HTTP ?
				"http://" + url : "https://" + url;
	return url;
}        


bool
isEquivalent(const string& u1, const string& u2)
{
    if (u1 == u2)
	return true;
    size_t i = 0;
    for (; i < u2.size() && u1[i] == u2[i]; ++i)
	;
    string suffix = u1.substr(i);
    if (suffix == "/")
	return true;
    if (suffix.size() >=6 && suffix.substr(0,6) == "index.")
	return true;
    
    return false;
					     
}

bool 
Url::isEquivalentTo(const Url& other) const
{
    if (other.domain() != this->domain())
	return false;
    else if (this->localAddr().size() > other.localAddr().size())
	return isEquivalent(toLower(this->localAddr()), 
			    toLower(other.localAddr()));
    else
	return isEquivalent(toLower(other.localAddr()), 
			    toLower(this->localAddr()));
}



string 
Url::subdomain(const std::string& domain) const
{
    vector<string> mainDomain;
    splitString(domain, '.', mainDomain);
    vector<string> thisDomain;
    splitString(mDomain, '.', thisDomain);

    size_t j = mainDomain.size();
    size_t i = thisDomain.size();
    while(i > 0 && j > 0 && mainDomain[j - 1] == thisDomain[i - 1]){
	--j;
	--i;
    }

    string ret;
    for (size_t k = 0; k < i; ++k){
	if (k > 0)
	    ret += ".";
	ret += thisDomain[k];
    }
    return ret;
}


Url::Url(std::string& fullUrl) : mProtocol(PAGE_PROTOCOL_HTTP)
{
    string unescsapedURL = unescapeUrl(fullUrl);

    extractDomain(unescsapedURL);

    size_t domainEndPos = toLower(unescsapedURL).find(mDomain) + mDomain.size();
    string page = unescsapedURL.substr(domainEndPos);
    if (page.size() == 0)
        page = "/";
    setFromLocalUrl(page);
}





bool 
operator == (const Url& u1, const Url& u2)
{
    return u1.fullAddr() == u2.fullAddr();
}

bool 
operator != (const Url& u1, const Url& u2)
{
    return !(u1 == u2);
}


