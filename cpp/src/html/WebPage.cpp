
#include "Logger.h"
#include "Config.h"
#include "WebPage.h"
#include "T_Account.hpp"
#include "WebDB.h"
#include "T_PageLink.hpp"
#include "T_PageImage.hpp"
#include "T_PageEmphasis.hpp"
#include "T_PageHeader.hpp"
#include "T_PageKeyword.hpp"
#include "T_IssuePage.hpp"
#include "ExcludedKeywords.h"
#include "Inserters.hpp"

using namespace DMMM;
using namespace std;


void
WebPage::regularize()
{
    I_Page pid = mPage._id();
    I_Account aid = mPage._account_id();
    setIds(mHeaders.begin(), mHeaders.end(), pid, aid);
    setIds(mLinks.begin(), mLinks.end(), pid, aid);
    setIds(mKeywords.begin(), mKeywords.end(), pid, aid);
    setIds(mImages.begin(), mImages.end(), pid, aid);
    setIds(mEmphases.begin(), mEmphases.end(), pid, aid);
    setIds(mIssues.begin(), mIssues.end(), pid, aid);
}

Url
pageUrl(const O_Page& page, const O_Account& account)
{
    string domain = 
        account._subdomain_option() == ACCOUNT_SUBDOMAIN_OPTION_NONE ?
        account._url():
        account._base_url();
    Url url;
    EnumPagePROTOCOL protocol = page._protocol();
    url.setFromProtocolDomainSubAndLocal(protocol, domain, 
                                         page._subdomain(),
                                         page._url());
    return url;
}

SiteWebPages
getFromPagesTable(I_Account accountId)
{
    LOG_INFO << "Getting page info for account " << accountId;
    
    SiteWebPages ret;

    T_Account t_a;
    O_Account account = t_a.select(accountId).first;

    string domain = 
        account._subdomain_option() == ACCOUNT_SUBDOMAIN_OPTION_NONE ?
        account._url():
        account._base_url();

    T_Page t_p(accountId);
    vector<O_Page> pages = t_p.select();
    
    ret.resize(pages.size());
    for (size_t i = 0; i < pages.size(); ++i){
	ret[i].mPage = pages[i];
        Url url;
	EnumPagePROTOCOL protocol = pages[i]._protocol();
        url.setFromProtocolDomainSubAndLocal(protocol, domain, 
					     pages[i]._subdomain(),
					     pages[i]._url());
        ret[i].mUrl = url;
    }
    return ret;
}


map<Url, O_Page>
getPagesInfo(I_Account accountId)
{
    map<Url, O_Page> ret;
    SiteWebPages webPages = getFromPagesTable(accountId);
    for (auto it = webPages.begin(); it != webPages.end(); ++it)
	ret[it->mUrl] = it->mPage;
    return ret;
}



class SortByRelevance{
public:
    bool operator () (const O_PageKeyword& k1, const O_PageKeyword& k2) const{
	return k1._relevance() > k2._relevance();
    }
};
vector<O_PageKeyword>
cull(const vector<O_PageKeyword>& keywords)
{
    ExcludedKeywords* excludedKeywords = ExcludedKeywords::instance();
    static size_t maxPerElement = confParam<size_t>("keywords.max_per_element");

    vector<O_PageKeyword> sorted = keywords;
    sort(sorted.begin(), sorted.end(), SortByRelevance());
    vector<O_PageKeyword> ret;

    map<EnumPageKeywordPOSITION, size_t> positionCount;
    for (size_t i = 0; i < sorted.size(); ++i){
	if(!excludedKeywords->allowed(sorted[i]._text()))
	    continue;
	EnumPageKeywordPOSITION position = sorted[i]._position();
	if(position != PAGEKEYWORD_POSITION_OVERALL &&
	   position != PAGEKEYWORD_POSITION_BACKLINK)
	    continue;
	++positionCount[position];
	if (positionCount[position] >= maxPerElement)
	    continue;
	ret.push_back(sorted[i]);
    }
    return ret;
}

bool 
insertParsedPages(const SiteWebPages& webPages, I_Account accountId)
{
    LOG_INFO << "Inserting " << webPages.size() 
	     << " webPages into DB for account " << accountId;

    size_t pageNum = 0;
    DBInserters inserters;
    for (auto it = webPages.begin(); it != webPages.end(); ++it, ++pageNum){
        WebPage wp = *it;
	wp.regularize();
	wp.mKeywords = cull(wp.mKeywords);
        inserters.add(wp);
    }

    static size_t chunk_size = confParam<size_t>("chunk_size");
    
    for(auto it = webPages.begin(); it != webPages.end(); ){
        DBFace::instance()->startTransaction(); 
        for (size_t j=0; j < chunk_size; ++j)
        {   
            O_Page p = it->mPage;
            p.update(); 
            ++it;
            if(it == webPages.end())
                break;
        }        
        DBFace::instance()->commitTransaction();
    }
    return true;
}

void
insertScores(SiteWebPages& rWebPages, I_Account accountId)
{
    LOG_INFO << "Inserting webPages into DB for account " << accountId;

    map<Url, O_Page> url2Info = getPagesInfo(accountId);

    WebDB* webDB = WebDB::instance();
    webDB->deleteAccountFromTable("issue_pages", accountId);

    static size_t chunk_size = confParam<size_t>("chunk_size");
    for(auto it = rWebPages.begin(); it !=rWebPages.end(); ){
        DBFace::instance()->startTransaction(); 
        for (size_t j=0; j < chunk_size; ++j)
        {   
            if(it == rWebPages.end())
                break;
 
            WebPage& wp = *it;
            if (url2Info.count(wp.mUrl) == 0){
                LOG_WARNING << "DB could not find " << wp.mUrl.localAddr();
                ++it;
                continue;
            }
            wp.mPage._id() = url2Info[wp.mUrl]._id();
            wp.mPage.update(); 
            setIds(wp.mIssues.begin(), wp.mIssues.end(), 
                   wp.mPage._id(), wp.mPage._account_id());
            ++it;
        }        
        DBFace::instance()->commitTransaction();
    }

    Inserter<T_IssuePage, O_IssuePage> issueInserter;
    for (auto it = rWebPages.begin(); it != rWebPages.end(); ++it){
        WebPage& wp = *it;
        if (url2Info.count(wp.mUrl) == 0){
            LOG_WARNING << "DB could not find " << wp.mUrl.localAddr();
            continue;
        }
	T_IssuePage t_issues(wp.mPage._id());
	issueInserter.add(wp.mIssues.begin(), wp.mIssues.end());
    }

}

