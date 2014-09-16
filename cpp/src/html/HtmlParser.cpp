#include <iostream>
#include <fstream>
#include <map>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_woarchive.hpp>
#include <boost/archive/text_wiarchive.hpp>
#include <iostream>
#include "dmmm_identifiers.hpp"
#include "Utils.hpp"
#include "Config.h"
#include "Logger.h"
#include "Utils.hpp"
#include "Inserters.hpp"
#include "ExcludedKeywords.h"
#include "HtmlParser.h"
#include "Keywords.h"
#include "HtmlParserObj.h"
#include "DetectLang.h"
#include "UTFConverter.h"
#include "RunInfo.h"
#include "O_PageImage.hpp"
#include "O_PageEmphasis.hpp"
#include "T_PageKeyword.hpp"
#include "T_PageLink.hpp"
#include "Phraser.h"
//#include <cctype>
//#include <wchar.h>
//#include <ctype.h>
#include <wctype.h>
#include <boost/bind.hpp>
#include "SlaveDriver.hpp"


using namespace boost::filesystem;
using namespace std;
namespace nsBFS = boost::filesystem;
using DMMM::O_PageImage;
using DMMM::O_PageEmphasis;
using DMMM::O_PageKeyword;
using DMMM::T_PageKeyword;
using DMMM::O_PageLink;
using DMMM::T_PageLink;
using DMMM::O_PageHeader;
using DMMM::O_Page;
using DMMM::O_Account;


std::set<std::string> HtmlParser::mStandAlones;

HtmlParser::HtmlParser(const string& text, SentenceCount const* sentenceCount,
                       PhraseBook const* phraseBook, size_t pos)
    : mTree(0), mText(text), mPos(pos), 
      mPhraseBook(phraseBook), mSentenceCount(sentenceCount) 
{ 
    mNode = &mTree;
}
        
void
HtmlParser::initializeStandAlones()
{
    if (mStandAlones.size() == 0){
	mStandAlones.insert("area");
	mStandAlones.insert("frame");
	mStandAlones.insert("link");
	mStandAlones.insert("base");
	mStandAlones.insert("hr");
	mStandAlones.insert("meta");
	mStandAlones.insert("basefont");
	mStandAlones.insert("img");
	mStandAlones.insert("param");
	mStandAlones.insert("br");
	mStandAlones.insert("input");
	mStandAlones.insert("col");
	mStandAlones.insert("isindex");
    }
}

void
HtmlParser::parse()
{
    while (mPos < mText.size())
           parseContent();

//    std::ofstream f("temp.txt");
//    mTree.nicePrint(f, 0);
}

// parse html files. current position points to plain text outside 
// tag parentheses 
void
HtmlParser::parseContent()
{
    string tag = mNode->tag();

    size_t nextPos = tag == "script" ?        
        min(mText.find("</script", mPos), mText.find("</SCRIPT", mPos)):
        mText.find_first_of("<", mPos);
    string content = nextPos != string::npos ? 
        mText.substr(mPos, nextPos - mPos) : string();
    
    if (content.size() && tag != "script" && tag != "style" && tag != "robots"){
        LOG_DEBUG << "Found content under last tag " << tag << ": " << content;
	mNode->addContentChild(content);
    }

    mPos = nextPos;
    if (mPos < mText.size()){
        ++mPos;
        parseOpenTag();
    }
}

string
HtmlParser::parseComment()
{
    string ret;
    size_t pos0 = mPos;
    if (mPos >= mText.size())
        return ret;    
    if (mPos + 2 >= mText.size() || 
        mText[mPos] != '-' || mText[mPos + 1] != '-')
    {
        mPos = min(mText.find(">", mPos), mText.size()) + 1;
        if (mPos > mText.size())
            LOG_DEBUG << "Malformed HTML - unclosed comment";
    }
    else{
        mPos = min(mText.find("-->", mPos), mText.size()) + 3;
        if (mPos > mText.size())
            LOG_DEBUG << "Malformed HTML - unclosed comment";
    }
    ret = string("<!") + mText.substr(pos0, mPos - pos0);
    return ret;
}
void
HtmlParser::parseOpenTag()
{
    if (mPos >= mText.size())
        return;    
    
    if (mText[mPos] == '!'){
        ++mPos;
//	mNode->addContentChild(parseComment());
	parseComment();
        return;
    }

    size_t nextPos = mText.find_first_of(">/ \n\r", mPos);
    string tag = nextPos != string::npos ? 
        mText.substr(mPos, nextPos - mPos) : 
        string();
    tag = toLower(tag);

    mPos = nextPos;
    if (mPos >= mText.size()){
        LOG_DEBUG << "Malformed HTML - unclosed tag";
        return;
    }
    else if (mText[mPos] == '>'){
        LOG_DEBUG << "Found tag: \"" << tag << "\"";
	mNode = mNode->addTagChild(tag);
        ++mPos;
    }
    else if (mText[mPos] == ' ' || mText[mPos] == '\n' || mText[mPos] == '\r'){
        LOG_DEBUG << "Found tag: \"" << tag << "\"";
	mNode = mNode->addTagChild(tag);
        ++mPos;
        parseTagParams(tag);
    }
    else if (mText[mPos] == '/'){
        ++mPos;
	mNode = mNode->addTagChild(tag);
	parseClosingTag();
    }
}

void
HtmlParser::parseClosingTag()
{
    if (mPos >= mText.size())
        return;

    size_t nextPos = min(mText.size(), mText.find_first_of(">", mPos));
    string tag = nextPos != mText.size() ? 
        mText.substr(mPos, nextPos - mPos) : 
        string();
    tag = trimSpaces(toLower(tag));
    LOG_DEBUG << "Found closing tag: " << tag;

    mPos = nextPos;
    if (mPos >= mText.size()){
        LOG_DEBUG << "Malformed HTML - unclosed tag";
	return;
    }
    else
	++mPos;

    
    for (HtmlNode* node = mNode; node->parent() != 0; node = node->parent()){
	if (tag == string("") || node->tag() == tag){
	    if (node != mNode)
		LOG_DEBUG << "Tag mismatch: " << mNode->tag() << " vs. " << tag;
	    else
		LOG_DEBUG << "Popping tag " << tag;
	    
	    mNode = node->parent();
	    break;
	}
    }
    LOG_DEBUG << "Tag mismatch: " << mNode->tag() << " vs. " << tag;

        
}

string
HtmlParser::parseTagParamsName()
{
    size_t nextPos = min(mText.size(), mText.find_first_of(">/=", mPos));
    string param = trimSpaces(toLower(mText.substr(mPos, nextPos - mPos)));
    mPos = nextPos;
    LOG_DEBUG << "Found param: " << param;
    
    return param;
}
string
HtmlParser::parseTagParamsValue()
{
    size_t nextPos = min(mText.size(), mText.find_first_of(" '\">", mPos));
    string str1 = trimSpaces(mText.substr(mPos, nextPos - mPos));
    mPos = nextPos;
    if (str1.size())
        return str1;
    else if (nextPos != mText.size()){
        char delim = mText[nextPos];
	++mPos;
        return parseQuote(delim);
    }
    else
        return string();
}

void
HtmlParser::parseTagParams(const string& tag)
{
    if (mPos >= mText.size())
        return;

    
    for (char lastDelim = ' '; lastDelim != '/' && lastDelim != '>'; ){
        string name = parseTagParamsName();
        if (mPos == mText.size()){
            LOG_DEBUG << "Malformed HTML - unclosed tag";
            break;
        }
        lastDelim = mText[mPos];
        if (lastDelim == '=')
            ++mPos;
        else
            break;
	while (mPos < mText.size() && mText[mPos] == ' ')
	    ++mPos;
        string value = parseTagParamsValue();
        lastDelim = mText[mPos];
        if (name.size() && value.size()){
	    mNode->insertParamVal(name, value);
            LOG_DEBUG << "Found param: " << name << " with value: " << value;
	    LOG_DEBUG << "Now at position " << mPos << ". view ahead: " 
		      << mText.substr(mPos, 30) << "...";
        }
    }
    LOG_DEBUG << "Done parsing params at position "
	      << mPos << ". view ahead: " 
	      << mText.substr(mPos, 30) << "...";
    if (mText[mPos] == '>'){
        ++mPos;
        if (isStandAlone(tag) && mNode->parent() != 0){
            LOG_DEBUG << "Popping tag " << mNode->tag();
            mNode = mNode->parent();
	}
    }
    else if (mText[mPos] == '/'){
        ++mPos;
	parseClosingTag();
    }
    else
        LOG_DEBUG << "Malformed HTML";
}

string
HtmlParser::parseQuote(char delim)
{
    string ret;
    if (mPos >= mText.size())
        return ret;

    size_t nextPos = min(mText.size(), mText.find(delim, mPos));
    ret = nextPos != mText.size() ?
        mText.substr(mPos, nextPos - mPos) :
        string();
    
    mPos = nextPos;
    if (mPos >= mText.size())
        LOG_DEBUG << "Malformed HTML - unclosed quote " << delim;
    else{
        LOG_DEBUG << "Found quote to pos " << mPos << ": " << ret;
        ++mPos;
    }
    return ret;
}


void
HtmlParser::extractCanonical(WebPage& rWebPage)
{
    LOG_DEBUG << "Extracting canonicals";

    HtmlNode::NodeCollection nodes;
    mTree.allTagsRecur("link", nodes);
    for (auto it = nodes.begin(); it != nodes.end(); ++it){
	const HtmlNode& n = **it;

        if (toLower(n["rel"]) == "canonical"){
            rWebPage.mHasCanonical = true;
            rWebPage.mCanonicalUrl.setFromLink(rWebPage.mUrl, n["href"]);
            LOG_DEBUG << "Canonical URL is: " 
                      << rWebPage.mCanonicalUrl.fullAddr();
            break;
        }
    }

}

void
HtmlParser::extractNoIndexFollow(WebPage& rWebPage)
{
    
    LOG_DEBUG << "Extracting NoIndex/NoFollow";
    HtmlNode::NodeCollection nodes;
    mTree.allTagsRecur("meta", nodes);
    for (auto it = nodes.begin(); it != nodes.end(); ++it){
	const HtmlNode& n = **it;

        if (toLower(n["name"]) == "robots"){
            string tags = toLower(n["content"]);
            if (tags.find("noindex") != string::npos)
                rWebPage.mNoIndex = true;
            if (tags.find("nofollow") != string::npos)
                rWebPage.mNoFollow = true;
        }

    }

}

void
HtmlParser::extractTitle(WebPage& rWebPage)
{
    LOG_DEBUG << "Extracting title";
    HtmlNode::NodeCollection nodes;
    mTree.allTagsRecur("title", nodes);

    rWebPage.mPage._page_title() = string();
    for (auto it = nodes.begin(); it != nodes.end(); ++it){
	const HtmlNode& n = **it;

        if (it != nodes.begin())
            rWebPage.mPage._page_title() += " ";
        rWebPage.mPage._page_title() += trimSpaces(n.content());
    }
}

void
HtmlParser::extractImages(WebPage& rWebPage)
{
    LOG_DEBUG << "Extracting images";
    HtmlNode::NodeCollection nodes;
    mTree.allTagsRecur("img", nodes);
    for (auto it = nodes.begin(); it != nodes.end(); ++it){
	const HtmlNode& n = **it;

        O_PageImage image(rWebPage.mPage._id());
        image._src() = trimSpaces(n["src"]);
        image._title() = trimSpaces(n["title"]);
        image._alt() = trimSpaces(n["alt"]);
        rWebPage.mImages.push_back(image);
    }

}

void
HtmlParser::extractEmphasis(WebPage& rWebPage, const string& tag,
			      const string& name)
{
    LOG_DEBUG << "Extracting " << name << " emphasis";
    HtmlNode::NodeCollection nodes;
    mTree.allTagsRecur(tag, nodes);
    for (auto it = nodes.begin(); it != nodes.end(); ++it){
	const HtmlNode& n = **it;

        O_PageEmphasis emph(rWebPage.mPage._id());
        emph._em_type() = name;
        emph._text() = trimSpaces(n.content());
        rWebPage.mEmphases.push_back(emph);
    }

}



path
removeRedundancies(const path& p)
{
    path ret;
    vector<path::const_iterator::value_type> tempRet;
    for (auto it = p.begin(); it != p.end(); ++it)
        if (*it == ".." && tempRet.size())
            tempRet.pop_back();
        else if (*it != ".")
            tempRet.push_back(*it);
    for (size_t i = 0; i < tempRet.size(); ++i)
        ret /= tempRet[i];
    return ret;
}

void
HtmlParser::extractLinks(WebPage& rWebPage, const map<Url, O_Page>& url2Info,
                         const Redirects& redirects)
{
    static size_t maxLinksPerPage = 
        confParam<size_t>("parser.max_links_per_page");

    HtmlNode::NodeCollection nodes;
    mTree.allTagsRecur("a", nodes);
    for (auto it = nodes.begin(); 
	 it != nodes.end() && rWebPage.mLinks.size() < maxLinksPerPage; 
	 ++it)
    {
	const HtmlNode& n = **it;

        bool nofollow = n["rel"].find("nofollow") != string::npos;

        string linkString = trimSpaces(n["href"]);
        Url linkUrl;
        if (!linkUrl.setFromLink(rWebPage.mBaseUrl, linkString))
            continue;
            

        O_PageLink link(rWebPage.mPage._id());
        link._href() = linkUrl.fullAddr();
        link._text() = trimSpaces(n.content());
        link._nofollow() = nofollow;
	link._is_image() = n.hasChildTag("img");
        link._relevant_text() = true;

        Url realTarget = linkUrl;
        auto itRed = redirects.find(realTarget);
        if (itRed != redirects.end() && itRed->second.second == 301)
            realTarget = itRed->second.first;
        auto itTarget = url2Info.find(realTarget);
        if (itTarget != url2Info.end())
            link._target_page_id() = itTarget->second._id().serialization();
        
        rWebPage.mLinks.push_back(link);	
    }

    LOG_TRACE << "Number of links found: " << rWebPage.mLinks.size();
}

void
HtmlParser::extractFrames(WebPage& rWebPage)
{
    LOG_DEBUG << "Extracting frames";

    HtmlNode::NodeCollection nodes;
    mTree.allTagsRecur("frame", nodes);
    for (auto it = nodes.begin(); it != nodes.end(); ++it){
	const HtmlNode& n = **it;

        string linkString = n["src"];
        Url frameUrl;
        if (!frameUrl.setFromLink(rWebPage.mBaseUrl, linkString))
            continue;
        if (frameUrl.domain() != rWebPage.mUrl.domain())
            continue;
        
        LOG_TRACE << "found frame: " << frameUrl.localAddr();
        rWebPage.mFrames.push_back(frameUrl);
    }
}

void
HtmlParser::extractHeaders(WebPage& rWebPage)
{
    LOG_DEBUG << "Extracting headers";
    for (size_t hType = 0; hType < 3; ++hType){
        string header("h");
        header += hType + '1';

	HtmlNode::NodeCollection nodes;
	mTree.allTagsRecur(header, nodes);
	for (auto it = nodes.begin(); it != nodes.end(); ++it){
	    const HtmlNode& n = **it;

            O_PageHeader header(rWebPage.mPage._id());
            header._text() = trimSpaces(n.content());
            header._h_type() = hType + 1;
            rWebPage.mHeaders.push_back(header);
	}

    }

}

void
HtmlParser::extractMeta(WebPage& rWebPage)
{
    LOG_DEBUG << "Extracting meta";

    HtmlNode::NodeCollection nodes;
    mTree.allTagsRecur("meta", nodes);
    for (auto it = nodes.begin(); it != nodes.end(); ++it){
	const HtmlNode& n = **it;

        const string& name = toLower(n["name"]);
        const string& content = n["content"];
        if (name == "description"){
            rWebPage.mPage._meta_description() = trimSpaces(content); 
            LOG_DEBUG << "Found description " 
                      << rWebPage.mPage._meta_description();
        }
        else if (name == "keywords"){
            rWebPage.mPage._meta_keywords() = trimSpaces(content); 
            LOG_DEBUG << "Found keywords " 
                      << rWebPage.mPage._meta_keywords();
        }
    }

}

void processKeywords(map<string, O_PageKeyword>& rKeywordMap, 
		     list <wstring>& keywordsBuffer, 
		     map<string, list<int> >& keywordPositionMap, 	
		     int keywordsCounter,  
		     EnumPageKeywordPOSITION location,
		     const PhraseBook& phraseBook)
{
    static bool filterOutNonPhrases = 
	confParam<bool>("keywords.filter_out_non_phrases");
    
    bool first;
    wstring keywordWString;
    string keywordString;
//    static ExcludedKeywords* excludedKeywords = ExcludedKeywords::instance();
    
    for (int i = keywordsBuffer.size() ; 0 < i ; i--) {
	first = true;
	keywordWString = L"";
	int j=i;
	
	vector<wstring> phrase;

	// create sentences
	for(auto it = keywordsBuffer.begin(); 0 < j ; it++, j--){
	    if (first) {
		first = false;
		keywordWString = *it;
	    } 
            else 
		keywordWString = *it + L" " + keywordWString;
	    
	    phrase.push_back(*it);
	}
	
	reverse(phrase.begin(), phrase.end());

	// check the sentence contains at least one alpha char
	if (checkStringForAlpha(keywordWString) )	{
	    keywordString =wstrToUtf8(keywordWString);

//	    if (phraseBook.size() == 0)
//		LOG_INFO << "EMPTY PHRASEBOOK";
	    if (phraseBook.size() && filterOutNonPhrases && phraseBook.count(phrase) == 0)
		continue;
/*	    
checking this in the phrase building section

	    // keyword is allowed
	    if (!excludedKeywords->allowed(keywordString)) {
		continue;
	    }
*/
	    
	    // create  position element for new keywords
	    if (keywordPositionMap.count(keywordString) == 0) 
		keywordPositionMap[keywordString] = list<int>();
	    
	    
	    // create new keyword element for new keywords
	    if (rKeywordMap.find(keywordString) == rKeywordMap.end() ) {
		rKeywordMap[keywordString]._relevance() = 0;
		rKeywordMap[keywordString]._frequency() = 0;
		rKeywordMap[keywordString]._text() = keywordString;
		rKeywordMap[keywordString]._size() = i;
		rKeywordMap[keywordString]._position() = location;
	    }
	    
	    ++rKeywordMap[keywordString]._frequency();
	    keywordPositionMap[keywordString].push_back(keywordsCounter-i);
	}
    }
}



void
countKeywords(const string& text0,  map<string, O_PageKeyword>& rKeywordMap,  
	      EnumPageKeywordPOSITION location, const PhraseBook& phraseBook)
{
    list <wstring> keywordsBuffer;
    map<string, list<int> > keywordPositionMap;
    
    static size_t minKeywordLen =
	confParam<size_t>("stats.min_keyword_length");
    static size_t maxWordsPerKeyword =
	confParam<size_t>("keywords.max_words_per_keyword");
    
    // debug - shlomi
    wstring wWord;
    wstring keywordWString;
    string keywordString;
    int keywordsCounter = 0;
    vector<string> keywords;
    wstring text = utf82wcs((text0 + string(" ")).c_str(),0);
    boost::to_lower(text);
    
    const wchar_t* char_array = text.c_str();
    while (*char_array){
	// collect word
	if (iswalnum (*char_array)) 
	    wWord += *char_array;
	else {
	    // word ended, process keywords
	    if (wWord.size() >= minKeywordLen) {
		keywordsCounter++;
		keywordsBuffer.push_front(wWord);
		
		if (keywordsBuffer.size() > maxWordsPerKeyword) 
		    keywordsBuffer.pop_back();

		processKeywords(rKeywordMap, keywordsBuffer,
				keywordPositionMap, keywordsCounter,
				location, phraseBook);
	    }
	    
	    wWord.clear();
	    if (*char_array != L' ' && *char_array != L'-' 
		&& *char_array != L'_')
	    {
		// keyword ended
		keywordsBuffer.clear();
	    }
	    
	}
/*
  replaced by code above

	if (*char_array == L'\n' || *char_array == L'\r' || 
	    *char_array == L',' || *char_array == L'.' || *char_array == L';')
	{
	    keywordsBuffer.clear();
	}
*/	
	if (*char_array == L'&')
	    do {
		char_array++;
	    }
	    while (char_array && (iswdigit(*char_array) || iswalpha(*char_array) || *char_array == L'#'));
	
	char_array++;
    }
    
    
}

void
countKeywords(const vector<string>& textVec, 
              map<string, O_PageKeyword>& rKeywordMap,
              EnumPageKeywordPOSITION location,
	      const PhraseBook& phraseBook)

{
    for (size_t i = 0; i < textVec.size(); ++i)
	countKeywords(textVec[i], rKeywordMap, location, phraseBook);
}


string 
locationString(EnumPageKeywordPOSITION location)
{
    string str;
    switch(location){
    case PAGEKEYWORD_POSITION_H1:
        str = "h1";
        break;
    case PAGEKEYWORD_POSITION_H2:
        str = "h2";
        break;
    case PAGEKEYWORD_POSITION_H3:
        str = "h3";
        break;
    case PAGEKEYWORD_POSITION_URL:
        str = "url";
        break;
    case PAGEKEYWORD_POSITION_DOMAIN:
        str = "domain";
        break;
    case PAGEKEYWORD_POSITION_TITLE:
        str = "title";
        break;
    case PAGEKEYWORD_POSITION_META_KWS:
        str = "meta_kws";
        break;
    case PAGEKEYWORD_POSITION_BODY:
        str = "body";
        break;
    case PAGEKEYWORD_POSITION_META_DESC:
        str = "meta_desc";
        break;
    case PAGEKEYWORD_POSITION_OVERALL:
        str = "overall";
        break;
    case PAGEKEYWORD_POSITION_BACKLINK:
        str = "backlink";
        break;
    default:
        break;
    }
    return str;
}

void
HtmlParser::extractKeywordsFromLocation(WebPage& rWebPage,
                                        EnumPageKeywordPOSITION location)
{
    LOG_TRACE << "Extracting keywords from " << location;

    HtmlNode::NodeCollection nodes;
    mTree.allTagsRecur(locationString(location), nodes);
    map<string, O_PageKeyword> keywordMap;


    set<string> excludedTags;
    excludedTags.insert("a");

    for (auto it = nodes.begin(); it != nodes.end(); ++it){
	const HtmlNode& n = **it;

        countKeywords(n.contentWithoutTags(excludedTags), keywordMap, 
		      location, *mPhraseBook);
    }
    
    for (auto it = keywordMap.begin(); it != keywordMap.end(); ++it)
        rWebPage.mKeywords.push_back(it->second);
    
}

void
HtmlParser::extractKeywordsFromBody(WebPage& rWebPage)
{
    LOG_TRACE << "Extracting keywords from body";

    HtmlNode::NodeCollection nodes;
    mTree.allTagsRecur(locationString(PAGEKEYWORD_POSITION_BODY), nodes);
    map<string, O_PageKeyword> keywordMap;

    set<string> excludedTags;
    excludedTags.insert("a");
    excludedTags.insert("h1");
    excludedTags.insert("h2");
    excludedTags.insert("h3");

    for (auto it = nodes.begin(); it != nodes.end(); ++it){
	const HtmlNode& n = **it;

        countKeywords(n.contentWithoutTags(excludedTags), 
		      keywordMap, PAGEKEYWORD_POSITION_BODY, *mPhraseBook);
    }
    
    for (auto it = keywordMap.begin(); it != keywordMap.end(); ++it)
        rWebPage.mKeywords.push_back(it->second);
    
}


string
replaceNonTextWithSpace(const string& s)
{
    wstring ret = utf8ToWStr(s);
    for (size_t i = 0; i < ret.size(); ++i)
        if (!iswalpha(ret[i]) && !iswdigit(ret[i]))
            ret[i] = L' ';
    return wstrToUtf8(ret);
}

void
HtmlParser::extractKeywordsFromURL(WebPage& rWebPage)
{
    LOG_DEBUG << "Extracting keywords from url";

    string url = rWebPage.mUrl.domain() + " " + rWebPage.mUrl.localAddr();
//    url = replaceNonTextWithSpace(url);

    map<string, O_PageKeyword> urlKeywordMap;
    countKeywords(url, urlKeywordMap, PAGEKEYWORD_POSITION_URL, *mPhraseBook);
    for (auto it = urlKeywordMap.begin(); it != urlKeywordMap.end(); ++it){
        it->second._relevance() = 100;
        rWebPage.mKeywords.push_back(it->second);
    }
}

void
HtmlParser::extractKeywordsFromDomain(WebPage& rWebPage)
{
    LOG_DEBUG << "Extracting keywords from domain";


    string domain = rWebPage.mUrl.domain();
//    domain = replaceNonTextWithSpace(domain);
    
    map<string, O_PageKeyword> domainKeywordMap;
    countKeywords(domain, domainKeywordMap, PAGEKEYWORD_POSITION_DOMAIN, 
		  *mPhraseBook);
    
    for (auto it = domainKeywordMap.begin();
         it != domainKeywordMap.end(); ++it)
    {
        it->second._relevance() = 100;
        rWebPage.mKeywords.push_back(it->second);
    }
}

void
getKeywordsWithRelevance(WebPage& rWebPage)
{
    static size_t keywordsPerPage = confParam<size_t>("keywords.num_per_page");

    WeightedKWs kws;
    calculateWeightedKeywords(rWebPage, kws, keywordsPerPage);
    for (auto it = kws.begin(); it != kws.end(); ++it){
        O_PageKeyword kw;
        kw._text() = it->first;
        kw._size() = it->second.mNumWords;
        kw._relevance() = it->second.mWeight;
        kw._position() = PAGEKEYWORD_POSITION_OVERALL;
        rWebPage.mKeywords.push_back(kw);
    }
}

void
HtmlParser::extractKeywordsFromMetaDesc(WebPage& rWebPage)
{
    LOG_DEBUG << "Extracting keywords from meta description";
    
    map<string, O_PageKeyword> metaDescKeywordMap;
    countKeywords(rWebPage.mPage._meta_description(), metaDescKeywordMap, 
                  PAGEKEYWORD_POSITION_META_DESC, *mPhraseBook);
    for (auto it = metaDescKeywordMap.begin();
         it != metaDescKeywordMap.end(); ++it)
    {
        rWebPage.mKeywords.push_back(it->second);
    }
}

void
HtmlParser::extractKeywordsFromMetaKeywords(WebPage& rWebPage)
{
    LOG_DEBUG << "Extracting keywords from meta keywords";

    map<string, O_PageKeyword> metaKeywordsKeywordMap;
    countKeywords(rWebPage.mPage._meta_keywords(), metaKeywordsKeywordMap, 
                  PAGEKEYWORD_POSITION_META_KWS, *mPhraseBook);
    for (auto it = metaKeywordsKeywordMap.begin();
         it != metaKeywordsKeywordMap.end(); ++it)
    {
        rWebPage.mKeywords.push_back(it->second);
    }
}

void
HtmlParser::extractKeywordsFromAltTags(WebPage& rWebPage)
{
    LOG_DEBUG << "Extracting keywords from alt tags";

    string altTags = mTree.contentFromTagParam("img","alt");

    map<string, O_PageKeyword> altTagsKeywordMap;
    countKeywords(altTags, altTagsKeywordMap, 
		  PAGEKEYWORD_POSITION_META_KWS, *mPhraseBook);
	
    for (auto it = altTagsKeywordMap.begin();
         it != altTagsKeywordMap.end(); ++it)
    {
        rWebPage.mKeywords.push_back(it->second);
    }
}

void
HtmlParser::extractKeywords(WebPage& rWebPage)
{

    extractKeywordsFromBody(rWebPage);
    extractKeywordsFromLocation(rWebPage, PAGEKEYWORD_POSITION_TITLE);
    extractKeywordsFromLocation(rWebPage, PAGEKEYWORD_POSITION_H1);
    extractKeywordsFromLocation(rWebPage, PAGEKEYWORD_POSITION_H2);
    extractKeywordsFromLocation(rWebPage, PAGEKEYWORD_POSITION_H3);
    extractKeywordsFromMetaDesc(rWebPage);
    extractKeywordsFromMetaKeywords(rWebPage);
    extractKeywordsFromDomain(rWebPage);
    extractKeywordsFromURL(rWebPage);
    extractKeywordsFromAltTags(rWebPage);
    getKeywordsWithRelevance(rWebPage);
}

void
HtmlParser::extractBaseUrl(WebPage& rWebPage)
{
    rWebPage.mBaseUrl = rWebPage.mUrl;


    HtmlNode::NodeCollection nodes;
    mTree.allTagsRecur("base", nodes);
    for (auto it = nodes.begin(); it != nodes.end(); ++it){
	const HtmlNode& n = **it;

        string base = n["href"];
        if (base.size()){
            Url newBase;
            newBase.setFromFullUrl(base);
            rWebPage.mBaseUrl = newBase;
        }
    }

    LOG_DEBUG << "Base URL: " << rWebPage.mBaseUrl.fullAddr();
            
}

void
HtmlParser::calculateUniqueContent(WebPage& rWebPage)
{
    Phraser<wstring> phraser;
    string content = 
        mTree.contentDelineated() + 
        mTree.contentFromTagParam("img", "alt");
    map<vector<wstring>, size_t> pageSentenceCount = 
        buildFromText(content, phraser);

    PhraseBook pagePhraseBook(phraser.getPhrases());

    
    size_t boilerPlateCount = 0;
    for (auto it = pageSentenceCount.begin(); it != pageSentenceCount.end(); 
         ++it)
    {   
        if (it->first.size() < 2)
            continue;
        size_t c = mSentenceCount->count(it->first);
        if (5*it->second >= c)
            rWebPage.mPage._unique_content_count() += it->first.size();
        else    
            boilerPlateCount += it->first.size();
    }
}


void
HtmlParser::extract(WebPage& rWebPage, const map<Url, O_Page>& url2Info,
                    const Redirects& redirects) 
{
    LOG_DEBUG << "Extracting interesting data";

/*
    if (mPhraseBook.size() == 0){
	Phraser<wstring> phraser;
	buildFromText(mTree.content(), phraser);
	phraser.addPhrases(mPhraseBook);
    }
*/
    extractBaseUrl(rWebPage);
    extractTitle(rWebPage);
    extractHeaders(rWebPage);
    extractMeta(rWebPage);
    extractImages(rWebPage);
    extractEmphasis(rWebPage, "em", "em");
    extractEmphasis(rWebPage, "strong", "strong");
    extractEmphasis(rWebPage, "b", "bold");
    extractEmphasis(rWebPage, "i", "italic");
    extractLinks(rWebPage, url2Info, redirects);
    extractFrames(rWebPage);
    extractKeywords(rWebPage);
//    extractScripts(rWebPage);
    extractCanonical(rWebPage);
    extractNoIndexFollow(rWebPage);
    calculateUniqueContent(rWebPage);
    rWebPage.mPage._language_id() = 
	LanguageDetector::instance()->language(mTree.content());
    string content = toLower(mTree.content());
    rWebPage.mPage._hashkey() = 
        hash32(content.begin(), content.end()).valForSerialization();
}



string
HtmlParser::textForClassifier() 
{
    return 
        extractContent("title") + 
        string(" ") +
        extractContent("body");
}


string
HtmlParser::extractContent(const string& tag)
{
    string ret;

    HtmlNode::NodeCollection nodes;
    mTree.allTagsRecur(tag, nodes);
    if (nodes.size() > 0){
	const HtmlNode& n = **(nodes.begin());
        ret = n.content();
    }
    
    return ret;
}



void
parseHtml(const string& text, const PhraseBook& phraseBook, 
          const SentenceCount& sentenceCount, 
          const map<Url, O_Page>& url2Info,
          const Redirects& redirects,
          WebPage& rWebPage)
{
//    string tidyText = tidyUp(text);
    HtmlParser parser(text, &sentenceCount, &phraseBook);
    parser.parse();
    parser.extract(rWebPage, url2Info, redirects);
}

void
parseHtml(const string& text, WebPage& rWebPage)
{
    parseHtml(text, PhraseBook(), SentenceCount(), map<Url, O_Page>(), 
              Redirects(), rWebPage);
                     
}

path
pathAfter(const path& full, const path& from)
{
    path retPath;
 
    bool foundFrom = false;
    for (auto it = full.begin(); it != full.end(); ++it){
        if (*it == from)
            foundFrom = true;
        if (foundFrom)
            retPath /= *it;
    }
    return retPath;
}

string
getPageHtmlFromFile(const I_Account accountId, Hash32 hash)
{
    path dir(RunInfo::instance()->mirrorDir(accountId));
    string fileName = hash.dbgval();
    nsBFS::ifstream pageFile(dir / hash.dbgval());
    if (!pageFile){
        LOG_ERROR << "Could not open" << dir / path(hash.dbgval());
        return string();
    }
    
    // testing - shlomi
    string pageSource((istreambuf_iterator<char>(pageFile)), 
		      istreambuf_iterator<char>());
    return pageSource;
}

bool 
parseHtmlPage(const Url& url, const I_Account accountId, 
              WebPage& rWebPage, Hash32 hash, size_t depth,
	      const PhraseBook& phraseBook,
              const SentenceCount& sentenceCount,
              const map<Url, O_Page>& url2Info,
              const Redirects& redirects)
{
    LOG_INFO << "Parsing " << url.localAddr();
    rWebPage.mUrl = url;
    rWebPage.mPage._depth() = depth;
    rWebPage.mPage._hashkey() = hash.valForSerialization();
    
    string pageSource = getPageHtmlFromFile(accountId, hash);

    parseHtml(pageSource, phraseBook, sentenceCount, url2Info, redirects,
              rWebPage);
    return true;
}

bool 
parseHtmlPage(const Url& url, const I_Account accountId, 
              WebPage& rWebPage, Hash32 hash, size_t depth,
	      const PhraseBook& phraseBook,
              const SentenceCount& sentenceCount)
{
    return parseHtmlPage(url, accountId, rWebPage, hash, depth, phraseBook,
                         sentenceCount, map<Url, O_Page>(), Redirects());
}

void
findAnchorKeywords(const O_Account account, const PhraseBook& phraseBook)
{
    LOG_INFO << "Finding anchor keywords";
    vector<O_PageLink> links = account._links(DMMM::T_PageLink::_target_page_id() > 0);
    map<I_Page, map<string, O_PageKeyword> > keywordMap;
    for (size_t i = 0; i < links.size(); ++i){
        I_Page tId = I_Page(links[i]._target_page_id());
        countKeywords(links[i]._text(), keywordMap[tId], 
                      PAGEKEYWORD_POSITION_BACKLINK, phraseBook);
    }
    Inserter<DMMM::T_PageKeyword, DMMM::O_PageKeyword> inserter;
    for (auto itP = keywordMap.begin(); itP != keywordMap.end(); ++itP){
        I_Page pid = itP->first;
        vector<O_PageKeyword> kws;
        for (auto it = itP->second.begin(); it != itP->second.end(); ++it){
            kws.push_back(it->second);
            kws.back()._page_id() = pid;
            kws.back()._account_id() = account._id();
        }
        inserter.add(kws.begin(), kws.end());
    }
}


bool
textIncludesKeywords(const string& s,
                     const vector<O_PageKeyword>& keywords, 
                     const set<EnumPageKeywordPOSITION>& locations)
{
    for (size_t i = 0; i < keywords.size(); ++i)
        if (locations.find(keywords[i]._position()) != locations.end() &&
            s.find(keywords[i]._text()) != string::npos)
        {
            return true;
        }
    return false;
}

/*
void
checkAnchorTextRelevance(SiteWebPages& rWebPages, 
                         const map<I_Page, WebPage*>& id2Page)
{
    LOG_INFO << "checking anchor text relevance";
    set<EnumPageKeywordPOSITION> keywordLocations;
    keywordLocations.insert(PAGEKEYWORD_POSITION_TITLE);
    keywordLocations.insert(PAGEKEYWORD_POSITION_H1);
    keywordLocations.insert(PAGEKEYWORD_POSITION_H2);
    keywordLocations.insert(PAGEKEYWORD_POSITION_H3);
    keywordLocations.insert(PAGEKEYWORD_POSITION_URL);
    for (auto itPage = rWebPages.begin(); itPage != rWebPages.end(); ++itPage){
        WebPage& wp = *itPage;
        for (size_t i = 0; i < wp.mLinks.size(); ++i){
            wp.mLinks[i]._relevant_text() = true;
            I_Page tId = I_Page(wp.mLinks[i]._target_page_id());
            if (tId != I_Page(0)){
                auto itP = id2Page.find(tId);
                if (itP == id2Page.end())
                    continue;
                WebPage* targetWP = itP->second;
                if (wp.mLinks[i]._text().size() > 1)
                    wp.mLinks[i]._relevant_text()
                        = textIncludesKeywords(toLower(wp.mLinks[i]._text()), 
                                               targetWP->mKeywords,
                                               keywordLocations);
            } 
        }
    }
}
*/

void
checkAnchorTextRelevance(const O_Account& account,
                         const map<Url, O_Page>& url2Info)
{
    LOG_INFO << "checking anchor text relevance";
    set<EnumPageKeywordPOSITION> keywordLocations = { PAGEKEYWORD_POSITION_OVERALL };

    vector<DMMM::I_PageLink> relevantLinks;
    for (auto it = url2Info.begin(); it != url2Info.end(); ++it){
        I_Page pageId = it->second._id();
        T_PageKeyword t_k(pageId);
        vector<O_PageKeyword> keywords = 
            t_k.select(t_k._position() == PAGEKEYWORD_POSITION_OVERALL);
        T_PageLink t_l;
        vector<O_PageLink> links = t_l.select(t_l._target_page_id() == 
                                          pageId.serialization());        

        for (size_t i = 0; i < links.size(); ++i)
            if (links[i]._text().size() > 1 && 
                textIncludesKeywords(toLower(links[i]._text()), 
                                     keywords, keywordLocations))
            {
                relevantLinks.push_back(links[i]._id());
            }
    }
    WebDB::instance()->updateLinkRelevance(account._id(), relevantLinks);
    
}


void
buildId2Page(SiteWebPages& rWebPages, 
             const map<Url, O_Page>& url2Info,
             map<I_Page, WebPage*>& rId2Page)
{
    for (auto itPage = rWebPages.begin(); itPage != rWebPages.end(); ++itPage){
        const WebPage& wp = *itPage;
        auto itUrl = url2Info.find(wp.mUrl);
        if (itUrl != url2Info.end())
            rId2Page[itUrl->second._id()] = &(*itPage);
    }   
}


I_Page
getNthId(const map<Url, O_Page>& url2Info, const size_t maxPages)
{
    vector<I_Page> ids;
    if (url2Info.size() == 0)
        return I_Page(0);

    for (auto it = url2Info.begin(); it != url2Info.end(); ++it)
	ids.push_back(it->second._id());

    sort(ids.begin(), ids.end());
    return ids[min(ids.size() - 1, maxPages - 1)];
}

void writeToArchive(boost::archive::text_woarchive& archive, 
                    const PhraseBook& phraseBook)
{
    map<Hash64::BaseType, size_t> mapPhraseBook;
    for (auto it = phraseBook.begin(); it != phraseBook.end(); ++it)
        mapPhraseBook.insert(make_pair(it->first.valForSerialization(),
                                       it->second));
    archive << mapPhraseBook;
}

void
writeToArchive(const I_Account accountId, const PhraseBook& phraseBook,
               const SentenceCount& sentenceCount)
{
    LOG_INFO << "writing phrasebook to archive";
    path archiveDir("tmp/phrasebooks/");
    if (!exists(archiveDir)){
        LOG_INFO << "Creating pharsebook directory";
        create_directory(archiveDir);
    }

    boost::filesystem::wofstream 
        archiveFile(archiveDir / (accountId.to_s() + ".arx"));
    boost::archive::text_woarchive archive(archiveFile);

    writeToArchive(archive, phraseBook);
    writeToArchive(archive, sentenceCount);
}

void readFromArchive(boost::archive::text_wiarchive& archive, 
                     PhraseBook& rPhraseBook)
{
    map<Hash64::BaseType, size_t> mapPhraseBook;
    archive >> mapPhraseBook;
    for (auto it = mapPhraseBook.begin(); it != mapPhraseBook.end(); ++it)
        rPhraseBook.insert(make_pair(it->first, it->second));
}

void
readFromArchive(const I_Account accountId, PhraseBook& rPhraseBook,
                SentenceCount& rSentenceCount)
{
    LOG_INFO << "reading phrasebook from archive";

    path archiveDir("tmp/phrasebooks/");
    if (exists(archiveDir / (accountId.to_s() + ".arx"))){
        boost::filesystem::wifstream
        archiveFile(archiveDir / (accountId.to_s() + ".arx"));
        boost::archive::text_wiarchive archive(archiveFile);

        readFromArchive(archive, rPhraseBook);
        readFromArchive(archive, rSentenceCount);
    }


    LOG_INFO << "found " << rPhraseBook.size() << " phrases and "
             << rSentenceCount.size() << " sentences";
}


class PagePhraseTask {
public:
    PagePhraseTask(const Url& url, const I_Account accountId, 
                   const Hash32 hash)
        : _url(&url), _accountId(accountId), _hash(hash)
        {}

    void run(){
	string text = getPageHtmlFromFile(_accountId, _hash);
	if (!text.empty()){
            SentenceCount sentenceCount;
            PhraseBook phraseBook;
            HtmlParser parser(text, &sentenceCount, &phraseBook);
            parser.parse();
            string content = _url->localAddr() + 
                parser.mTree.contentDelineated() + 
                parser.mTree.contentFromTagParam("img", "alt");
            _sentenceCount = SentenceCount(buildFromText(content, _phraser));
	}

    }

public:
    Phraser<wstring>        _phraser;
    SentenceCount           _sentenceCount;

private:
    Url const*              _url;
    I_Account               _accountId;
    Hash32                  _hash;
};


void
buildSiteContentStats(const I_Account accountId, 
                      const map<Url, O_Page>& url2Info,
                      PhraseBook& rPhraseBook,
                      SentenceCount& rSentenceCount)
{
    LOG_INFO << "building phrasebook for account " << accountId;
    

    vector<PagePhraseTask> pagePhraseTasks;
    for (auto it = url2Info.begin(); it != url2Info.end(); ++it)
        pagePhraseTasks.push_back(
            PagePhraseTask(it->first, accountId, 
                           Hash32(it->second._hashkey())));

    driveSlaves(confParam<size_t>("parser.num.threads"), 
                pagePhraseTasks.begin(), pagePhraseTasks.end(),
                pagePhraseTasks.size(), "build phrasebook");

    LOG_INFO << "uniting phrasebooks";
    Phraser<wstring> phraser;
    for (size_t i = 0; i < pagePhraseTasks.size(); ++i){
        rSentenceCount.add(pagePhraseTasks[i]._sentenceCount);
        phraser.addPhraser(pagePhraseTasks[i]._phraser);
    }
    LOG_INFO << "extracting phrases";
    rPhraseBook = phraser.getPhrases();

    writeToArchive(accountId, rPhraseBook, rSentenceCount);
    LOG_INFO << "phrases found: " << rPhraseBook.size();
}

class KeywordsTask {
public:
    KeywordsTask(const Url& url, const O_Page& page, 
                 const PhraseBook& phraseBook, SiteKeywords& rKeywords,
                 boost::mutex& mutex)
        : _phraseBook(&phraseBook), _siteKeywords(&rKeywords), _mutex(&mutex)
        {
            _wp.mPage = page;
            _wp.mUrl = url;
        }

    void run(){
        LOG_INFO << "Parsing " << _wp.mUrl.localAddr();
	string text = getPageHtmlFromFile(_wp.mPage._account_id(), 
                                          Hash32(_wp.mPage._hashkey()));
        SentenceCount sentenceCount;
	HtmlParser parser(text, &sentenceCount, _phraseBook);
	parser.parse();
	parser.extractKeywords(_wp);
        boost::mutex::scoped_lock lock(*_mutex);
	_siteKeywords->push_back(PageKeywordsInfo(_wp.mPage, 
                                                  _wp.mUrl, 
                                                  _wp.mKeywords));
    }

private:
    WebPage                 _wp;
    PhraseBook const*       _phraseBook;
    SiteKeywords*           _siteKeywords;
    boost::mutex*           _mutex;
};
		

SiteKeywords
getSiteKeywords(I_Account accountId)
{
    LOG_INFO << "Getting site keywords for " << accountId;
    SiteKeywords ret;

    string dir = RunInfo::instance()->mirrorDir(accountId);
    
    if (!exists(dir)){
        LOG_WARNING << dir << " doesn't exist";
        return ret;
    }
    
    map<Url, O_Page> url2Info = getPagesInfo(accountId); 
    Redirects redirects = WebDB::instance()->getRedirects(accountId);

    PhraseBook phraseBook;
    SentenceCount sentenceCount;
    readFromArchive(accountId, phraseBook, sentenceCount);


    size_t maxPages = confParam<size_t>("keywords.max_pages_for_suggestions");
    I_Page maxId = getNthId(url2Info, maxPages);

    vector<KeywordsTask> keywordsTasks;
    boost::mutex keywordsMutex;
    for (auto it = url2Info.begin(); it != url2Info.end(); ++it)
	if (it->second._id() <= maxId)
            keywordsTasks.push_back(KeywordsTask(it->first, it->second, 
                                                 phraseBook, ret,
                                                 keywordsMutex));
    
    driveSlaves(confParam<size_t>("parser.num.threads"), 
                keywordsTasks.begin(), keywordsTasks.end(),
                keywordsTasks.size(), "build site keywords");

    return ret;
}

class PageTask {
public:
    PageTask(const Url& url, const O_Page& page, I_Account accountId,
             const PhraseBook& phraseBook, const SentenceCount& sentenceCount,
             DBInserters& dbInserters, const map<Url, O_Page>& url2Info,
             const Redirects& redirects)
        : _url(&url), _page(page), _accountId(accountId), 
          _phraseBook(&phraseBook), _sentenceCount(&sentenceCount),
          _dbInserters(&dbInserters), _url2Info(&url2Info), 
          _redirects(&redirects)
    {}

    void run(){
        WebPage wp;
        wp.mPage = _page;
        parseHtmlPage(*_url, _accountId, wp, Hash32(_page._hashkey()), 
                      _page._depth(), *_phraseBook, *_sentenceCount,
                      *_url2Info, *_redirects);
	wp.regularize();
	wp.mKeywords = cull(wp.mKeywords);
        _dbInserters->add(wp);
        _page = wp.mPage;
    }

    void updatePage(){
        _page.update();
    }
 
private:
    Url const*              _url;
    O_Page                  _page;
    I_Account               _accountId;
    PhraseBook const*       _phraseBook;
    SentenceCount const*    _sentenceCount;
    DBInserters*            _dbInserters;
    map<Url, O_Page> const* _url2Info;
    Redirects const*        _redirects;
};


void
parseSite(const O_Account& account)
{
    LOG_INFO << "Parsing " << account._url();
    I_Account accountId = account._id();

    string dir = RunInfo::instance()->mirrorDir(accountId);
    if (!exists(dir)){
        LOG_INFO << dir << " doesn't exist";
        return;
    }

    map<Url, O_Page> url2Info = getPagesInfo(accountId); 
    Redirects redirects = WebDB::instance()->getRedirects(accountId);

    PhraseBook phraseBook;
    SentenceCount sentenceCount;
    buildSiteContentStats(accountId, url2Info, phraseBook, sentenceCount);

    vector<PageTask> pageTasks;
    DBInserters dbInserters;
    for (auto it = url2Info.begin(); it != url2Info.end(); ++it)
        pageTasks.push_back(PageTask(it->first, it->second, accountId,
                                     phraseBook, sentenceCount,
                                     dbInserters, url2Info, redirects));     
    driveSlaves(confParam<size_t>("parser.num.threads"), 
                pageTasks.begin(), pageTasks.end(),
                pageTasks.size(), "parse site");

    static size_t chunk_size = confParam<size_t>("chunk_size");
    for(size_t i = 0; i < pageTasks.size(); ){
        DMMM::DBFace::instance()->startTransaction(); 
        for(size_t j=0; j < chunk_size; ++j)
        {   
            pageTasks[i].updatePage();
            ++i;
            if(i >= pageTasks.size())
                break;
        }        
        DMMM::DBFace::instance()->commitTransaction();
    }
    findAnchorKeywords(account, phraseBook);
    checkAnchorTextRelevance(account, url2Info);        
}


class ParseTask {
public:
    ParseTask(const Url& url, const O_Page& page, I_Account accountId,
              const PhraseBook& phraseBook, const SentenceCount& sentenceCount)
        : _url(&url), _accountId(accountId), 
          _phraseBook(&phraseBook), _sentenceCount(&sentenceCount)
    {
        _webPage.mPage = page;
    }

    void run(){
        parseHtmlPage(*_url, _accountId, _webPage, 
                      Hash32(_webPage.mPage._hashkey()), 
                      _webPage.mPage._depth(), *_phraseBook, *_sentenceCount);
    }

public:
    WebPage              _webPage;
    
private:
    Url const*           _url;
    I_Account            _accountId;
    PhraseBook const*    _phraseBook;
    SentenceCount const* _sentenceCount;
    
};


SiteWebPages
parseHtmlSite(const O_Account& account)
{
    LOG_INFO << "Parsing Html Site " << account._url();

    I_Account accountId = account._id();

    SiteWebPages webPages;
    double progress = 0.0;
    
    string dir = RunInfo::instance()->mirrorDir(accountId);

    if (!exists(dir)){
        LOG_INFO << dir << " doesn't exist";
        return webPages;
    }

    WebDB* webDB = WebDB::instance();
    map<Url, O_Page> url2Info = getPagesInfo(accountId); 

    PhraseBook phraseBook;
    SentenceCount sentenceCount;
    buildSiteContentStats(accountId, url2Info, phraseBook, sentenceCount);

    vector<ParseTask> parseTasks;

    for (auto it = url2Info.begin(); it != url2Info.end(); ++it)
        parseTasks.push_back(ParseTask(it->first, it->second, accountId,
                                       phraseBook, sentenceCount));
    driveSlaves(confParam<size_t>("parser.num.threads"), 
                parseTasks.begin(), parseTasks.end(), parseTasks.size());

    webPages.resize(parseTasks.size());
    for (size_t i = 0; i < parseTasks.size(); ++i)
        webPages[i] = parseTasks[i]._webPage;
    


    map<I_Page, WebPage*> id2Page;
    buildId2Page(webPages, url2Info, id2Page);

    //findInternalBacklinks(webPages, url2Info);
    progress += 1.0;
    webDB->updateMirrorProgress(accountId, (size_t) progress);
    //checkAnchorTextRelevance(webPages, id2Page);
    progress += 1.0;
    webDB->updateMirrorProgress(accountId, (size_t) progress);
    //findAnchorKeywords(webPages, url2Info, id2Page, phraseBook);
    progress += 2.0;
    webDB->updateMirrorProgress(accountId, (size_t) progress);
    for (auto it = webPages.begin(); it != webPages.end(); ++it){
    	try{
    		getKeywordsWithRelevance(*it);
    		progress += 6.0 / url2Info.size();
//    		webDB->updateMirrorProgress(accountId, (size_t) progress);
    	}
    	catch(exception& e){
    		LOG_ERROR << e.what();
    	}
    }
    
    return webPages;
}

