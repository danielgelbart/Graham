#ifndef HTMLPARSER_H
#define HTMLPARSER_H

#include <string>
#include <boost/filesystem.hpp>
#include <boost/unordered_map.hpp>
#include "WebPage.h"
#include "WebDB.h"


std::string
getPageHtmlFromFile(const DMMM::I_Account accountId, Hash32 hash);

class PhraseBook{
public:
    PhraseBook() {} 
    PhraseBook(const std::map<std::vector<std::wstring>, size_t>& p)
    {
        for (auto it = p.begin(); it != p.end(); ++it)
            _phraseCounts[hash64(it->first)] += it->second;
    }

    size_t count(const std::vector<std::wstring>& phrase) const{
        auto itFind = _phraseCounts.find(hash64(phrase));
        if (itFind == _phraseCounts.end())
            return 0;
        else
            return itFind->second; 
    }

    size_t size() const { return _phraseCounts.size(); }

//    boost::unordered_map<Hash64, size_t>::const_iterator begin() const
    std::map<Hash64, size_t>::const_iterator begin() const
    { return _phraseCounts.begin(); }

//    boost::unordered_map<Hash64, size_t>::const_iterator end() const
    std::map<Hash64, size_t>::const_iterator end() const
    { return _phraseCounts.end(); }

    bool insert(const std::pair<Hash64::BaseType, size_t>& kv)
    {
        return _phraseCounts.insert(std::make_pair(Hash64(kv.first), 
                                                   kv.second)).second;
    }
    
    void add(const PhraseBook& p)
    {
        addToHistogram(_phraseCounts, p._phraseCounts);
    }

private:
//    boost::unordered_map<Hash64, size_t> _phraseCounts;
    std::map<Hash64, size_t> _phraseCounts;
};

//typedef boost::unordered_map<std::vector<std::wstring>, size_t> PhraseBook;
//typedef std::map<std::vector<std::wstring>, size_t> SentenceCount;
typedef PhraseBook SentenceCount;


SiteKeywords
getSiteKeywords(const DMMM::I_Account accountId);

void
parseHtml(const std::string& text, const PhraseBook& phraseBook,
          const SentenceCount& sentenceCount, 
          const std::map<Url, DMMM::O_Page>& url2Info,
          const Redirects& redirects,
          WebPage& rWebPage);

bool 
parseHtmlPage(const Url& url, const DMMM::I_Account accountId, 
              WebPage& rWebPage, Hash32 hash, size_t depth,
	      const PhraseBook& phraseBook,
              const SentenceCount& sentenceCount);

void
parseHtml(const std::string& text, WebPage& rWebPage);


bool
relativize(const std::string& linkString, 
           const boost::filesystem::path& page, 
           boost::filesystem::path& rLinkPath,
           const std::string& siteUrl);

void readFromArchive(const DMMM::I_Account accountId, PhraseBook& rPhraseBook,
                     SentenceCount& rSentenceCount);

#endif //HTMLPARSER_H

