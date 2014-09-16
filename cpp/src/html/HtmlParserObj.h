#ifndef HTML_PARSER_OBJ_H
#define HTML_PARSER_OBJ_H
#include <set>
#include "WebPage.h"
#include "WebDB.h"
#include "HtmlNode.h"
#include "HtmlParser.h"

class HtmlParser{

public:
    HtmlParser(const std::string& text, SentenceCount const* sentenceCount,
               PhraseBook const* phraseBook, size_t pos = 0);

    // create DOM in mTree
    void parse();

    void extract(WebPage& rWebPage,
                 const std::map<Url, DMMM::O_Page>& url2Info,
                 const Redirects& redirects);
    void extractKeywords(WebPage& rWebPage);

    std::string extractContent(const std::string& tag);
    std::string textForClassifier();


    static bool isStandAlone(const std::string& tag) {
	if (mStandAlones.size() == 0)
	    initializeStandAlones();
	return mStandAlones.count(tag) == 1;
    }
    

private:
    void   parseContent();
    void   parseOpenTag();
    void   parseClosingTag();
    std::string parseQuote(char delim);
    std::string parseComment();
    void   parseTagParams(const std::string& tag);
    std::string parseTagParamsName();
    std::string parseTagParamsValue();
    
    void   extractBaseUrl(WebPage& rWebPage); 
    void   extractTitle(WebPage& rWebPage); 
    void   extractHeaders(WebPage& rWebPage);
    void   extractMeta(WebPage& rWebPage);
    void   extractImages(WebPage& rWebPage);
    void   extractLinks(WebPage& rWebPage,
                        const std::map<Url, DMMM::O_Page>& url2Info,
                        const Redirects& redirects);
    void   extractFrames(WebPage& rWebPage);
    void   extractEmphasis(WebPage& rWebPage, const std::string& tag,
			   const std::string& name);
//    void   extractScripts(WebPage& rWebPage);
    void   extractKeywordsFromLocation(WebPage& rWebPage,
                                       EnumPageKeywordPOSITION location);
    void   extractKeywordsFromBody(WebPage& rWebPage);
    void   extractKeywordsFromURL(WebPage& rWebPage);
    void   extractKeywordsFromDomain(WebPage& rWebPage);
    void   extractKeywordsFromMetaDesc(WebPage& rWebPage);
    void   extractKeywordsFromMetaKeywords(WebPage& rWebPage);
    void   extractKeywordsFromAltTags(WebPage& rWebPage);

    void   extractCanonical(WebPage& rWebPage);
    void   extractNoIndexFollow(WebPage& rWebPage);

    void   calculateUniqueContent(WebPage& rWebPage);


    static void initializeStandAlones();
    PhraseBook const* phraseBook() const { return mPhraseBook; }
    SentenceCount const* sentenceCount() const { return mSentenceCount; }
    
public:
    HtmlNode                           mTree;

private:
    const std::string&                 mText;
    size_t                             mPos;
    HtmlNode*                          mNode;
    PhraseBook const*                  mPhraseBook;
    SentenceCount const*               mSentenceCount;

    static std::set<std::string>       mStandAlones;
};

#endif //HTML_PARSER_OBJ_H
