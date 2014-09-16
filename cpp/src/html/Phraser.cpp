#include <iostream>
#include <set>
#include <boost/algorithm/string.hpp>
#include "ExcludedKeywords.h"
#include "Logger.h"
#include "Phraser.h"
#include "HtmlParser.h"
#include "Utils.hpp"

using namespace std;

typedef std::vector<wstring> Sentence;
typedef std::vector<Sentence> Document;



void
buildSentences(const string& text0, Document& rDoc)
{
    
    wstring wWord;
    vector<wstring> sentence;
    wstring text = utf82wcs((text0 + string(" ")).c_str(), 0);
    boost::to_lower(text);

    const wchar_t* char_array = text.c_str();
    while (*char_array){
	// collect word
	if (iswalnum (*char_array)) 
	    wWord += *char_array;
	else {
	    // word ended, process keywords
	    if (wWord.size() > 0){ 
		sentence.push_back(wWord);
		wWord.clear();
	    }
	    if (*char_array != L' ' && *char_array != L'-' 
		&& *char_array != L'_' && *char_array != L'\'')
	    {
		// sentence ended
		if (sentence.size() > 0){
		    if (sentence.size() < 40)
			rDoc.push_back(sentence);		    
		    sentence.clear();
		}
	    }
	    
	}
	if (*char_array == L'&')
	    do {
		char_array++;
	    }
	    while (char_array && 
		   (iswdigit(*char_array) || 
		    iswalpha(*char_array) || 
		    *char_array == L'#'));
	
	char_array++;
    }
}


string
vecwToString(const vector<wstring>& vw)
{
    string ret;
    for (size_t i = 0; i < vw.size(); ++i){
	if (i > 0)
	    ret += " ";
	ret += wstrToUtf8(vw[i]);
    }
    return ret;
}

bool 
allowed(const vector<wstring>& phrase)
{
    static ExcludedKeywords* excludedKeywords = ExcludedKeywords::instance();
    return excludedKeywords->allowed(vecwToString(phrase));
}

map<vector<wstring>, size_t> 
buildFromText(const string& text, Phraser<wstring>& p)
{
    Document doc;
    buildSentences(text, doc);

    for (size_t i = 0; i < doc.size(); ++i)
	p.addSentence(doc[i]);
    
    return histogram(doc);
}


void
buildPhrases(const string& text0)
{
    Phraser<wstring> phraser;
    Document doc;
    buildSentences(text0, doc);

    for (size_t i = 0; i < doc.size(); ++i)
	phraser.addSentence(doc[i]);

    std::map<vector<wstring>, size_t> phrases = phraser.getPhrases();


}

template<>
void
removeNotAllowed<wstring>(std::map<std::vector<wstring>, size_t>& r)
{
    typename std::map<std::vector<wstring>, size_t>::iterator itNext = 
        r.begin();
    for(typename std::map<std::vector<wstring>, size_t>::iterator it = itNext; 
	    it != r.end(); it = itNext)
    {
	++itNext;
	if (!allowed(it->first))
	    r.erase(it);
    }
}
