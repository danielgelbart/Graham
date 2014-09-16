#ifndef WEBPAGES_H
#define WEBPAGES_H

#include <string>
#include <vector>
#include <map>
#include "Utils.hpp"
#include "Url.h"
#include "GlobalIds.h"
#include "dmmm_identifiers.hpp"
#include "O_Account.hpp"
#include "O_Page.hpp"
#include "O_PageHeader.hpp"
#include "O_PageLink.hpp"
#include "O_PageKeyword.hpp"
#include "O_PageEmphasis.hpp"
#include "O_PageImage.hpp"
#include "O_IssuePage.hpp"

struct WebPage
{
    WebPage() 
	: mHasCanonical(false), mNoIndex(false), 
	  mNoFollow(false), mMaxImpact(0.0) 
	{}
    DMMM::O_Page                        mPage;
    std::vector<DMMM::O_PageHeader>     mHeaders;
    std::vector<DMMM::O_PageLink>           mLinks;
    std::vector<DMMM::O_PageKeyword>        mKeywords;
    std::vector<DMMM::O_PageEmphasis>       mEmphases;
    std::vector<DMMM::O_PageImage>          mImages;
    std::vector<DMMM::O_IssuePage>      mIssues;
    
    bool   mHasCanonical;
    Url    mCanonicalUrl;
    Url    mUrl;
    Url    mBaseUrl;
    bool   mNoIndex;
    bool   mNoFollow;
    double mMaxImpact;
    std::vector<Url>                    mFrames;
    
    void regularize(); //set the page_id and account_id of the subclasses
};



typedef std::vector<WebPage> SiteWebPages;

Url pageUrl(const DMMM::O_Page& page, const DMMM::O_Account& account);

bool insertParsedPages(const SiteWebPages& webPages, DMMM::I_Account accountId);
std::map<Url, DMMM::O_Page> getPagesInfo(DMMM::I_Account accountId);
void insertScores(SiteWebPages& rWebPages, DMMM::I_Account accountId);
std::vector<DMMM::O_PageKeyword> cull(const std::vector<DMMM::O_PageKeyword>& keywords);




struct LessThanByTitle{
    bool operator() (const WebPage& page1, const WebPage& page2){
        return page1.mPage._page_title() < page2.mPage._page_title();
    }
};

struct LessThanByAboveRank{
    bool operator() (const WebPage& page1, const WebPage& page2){
        return page1.mPage._above_rank() < page2.mPage._above_rank();
    }
};

struct MoreThanByAboveRank{
    bool operator() (const WebPage& page1, const WebPage& page2){
        return page1.mPage._above_rank() > page2.mPage._above_rank();
    }
};

template<class I>
void
setIds(I begin, I end, DMMM::I_Page pid,  DMMM::I_Account aid)
{
    while(begin != end){
	begin->_page_id() = pid;
	begin->_account_id() = aid;
	++begin;
    }
} 

#endif //WEBPAGES_H
