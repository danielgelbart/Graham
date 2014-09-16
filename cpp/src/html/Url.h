#ifndef URL_H
#define URL_H

#include <string>
#include <curl/curl.h>

enum EnumPagePROTOCOL { PAGE_PROTOCOL_HTTP = 1, PAGE_PROTOCOL_HTTPS = 2 };

// MASTER
class Url{
public:

    Url() : mProtocol(PAGE_PROTOCOL_HTTP) {}
    Url(std::string& fullUrl);

    const std::string& domain() const { return mDomain; }
    const std::string& path() const { return mPath; }
    const std::string& leaf() const { return mLeaf; }
    const std::string& params() const { return mParams; }
    EnumPagePROTOCOL protocol() const { return mProtocol; }

    // calculates the subdomain given the domain
    std::string subdomain(const std::string& domain) const;

    std::string localAddr() const { return mPath + mLeaf + mParams; }
    std::string fullAddr() const;

    bool setFromFullUrl(const std::string& fullUrl);
    bool setFromLink(const Url& url, const std::string& link);
    bool setFromDomainAndLocal(const std::string& domain, 
                               const std::string& localUrl);
    bool setFromProtocolDomainSubAndLocal(EnumPagePROTOCOL protocol,
					  const std::string& domain, 
					  const std::string& subdomain,
					  const std::string& localUrl);
    void setProtocol(EnumPagePROTOCOL p) { mProtocol = p; }
    bool operator < (const Url& other) const{
        return fullAddr() < other.fullAddr();
    }

    bool isEquivalentTo(const Url& other) const;

private:
    bool setFromRelativeLink(const Url& url, const std::string& link);
    bool setFromLocalUrl(const std::string& localUrl);

    void regularizeDomain();
    void extractDomain(const std::string& url);

    bool isLegal() const;
    void trimParams();

private:
    EnumPagePROTOCOL       mProtocol;
    std::string            mDomain;
    std::string            mPath;
    std::string            mLeaf;
    std::string            mParams;
};

bool operator == (const Url& u1, const Url& u2);
bool operator != (const Url& u1, const Url& u2);

#endif //URL_H
