#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <string>
#include "types.h"

using namespace std;

class Tokenizer {

public:

    Tokenizer(string& text) {
        _text = text;
        _pos = 0;
    }

    /* return the next segment delimited by the string "<DOCUMENT>"
       advances the _pos marker */
    string getNextDelString(string& delimiter);
    size_t getNextDocPos();
    string findFilingSummary();
    void getReportDocNames(map<ReportType,string>* reports_map);
    string extractTagContent(string& tagName,string& content);
    void parseToDocuments();
    string findDoc(string& docName);

private:
    string readReportHtmlNameFromFS(string& report);

private:
    string _text;
    size_t _pos;
    //store the starting position of each 'DOCUMENT' from text file
    vector<size_t> _doc_poses;
};

#endif //TOKENIZER_H





