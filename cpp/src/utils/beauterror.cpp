#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

using namespace std;

size_t indentSize = 4;
size_t maxWidth = 80;

template<char openParen, char closeParen>
bool
findMatchingParenths(const string& s, 
                     map<size_t, size_t>& rMatchingParenths)
{
    vector<size_t> pStack;
    for (size_t i = 0; i < s.size(); ++i){
        char c = s[i];
        switch(c){
        case openParen: 
            pStack.push_back(i);
            break;
        case closeParen:
            if (pStack.size() == 0)
                return false;
            rMatchingParenths.insert(make_pair(pStack.back(), i));
            pStack.pop_back();
            break;
        default:
            break;
        }
        
    }
    return pStack.size() == 0;
        
}

void
newLine(string& r, size_t& rCol, size_t indent)
{
    r += '\n';
    r += string(indent, ' ');
    rCol = indent;
}

string
reformat(const string& s)
{
    map<size_t, size_t> matchingParenths;
    if (!findMatchingParenths<'<', '>'>(s, matchingParenths))
        return s;
    if (!findMatchingParenths<'(', ')'>(s, matchingParenths))
        return s;
    if (!findMatchingParenths<'[', ']'>(s, matchingParenths))
        return s;
    
    string r;
    size_t indent = 0;
    size_t col = 0;

    for (size_t i = 0; i < s.size(); ++i){
        if (s.size() - i < maxWidth - col){
            r += s.substr(i);
            break;
        }
        char c = s[i];
        switch(c){

        //open parentheses
        case '[':
        case '(':
        case '<': {
            size_t j = matchingParenths[i];

            //include the whole block if possible
            if (j != 0 && col + j + 1 - i <= maxWidth){
                r += s.substr(i, j + 1 - i);
                col += j + 1 - i;
                i = j;
            }
            else{
                r += c;
                indent += indentSize;
                newLine(r, col, indent);
            }
        }
            break;

        //close parentheses
        case ']':
        case ')':
        case '>':
            indent -= indentSize;
            newLine(r, col, indent);
            r += c;
            ++col;
            if (i < s.size() - 1 && s[i + 1] == ','){
                r += s[++i];
                ++col;
                if (i < s.size() - 1 && s[i + 1] == ' '){
                    r += s[++i];
                    ++col;
                }
                newLine(r, col, indent);
            }
            break;

        //begin/end quote    
        case -30:
            r += c;
            ++col;
            if (i < s.size() - 2){
                r += s[++i];
                r += s[++i];
                newLine(r, col, indent);
            }
            break;
//         case ',':
//             r += c;
//             ++col;
//             {
//                 char cPrev = i > 0 ? s[i - 1] : 0;
//                 if (i < s.size() - 1 && s[i + 1] == ' '){
//                     r += s[++i];
//                     ++col;
//                 }
//                 if (cPrev == '>')
//                     newLine(r, col, indent);
//             }
//             break;
        default:
            r += c;
            ++col;
            break;
        }
        if (col >= maxWidth)
            newLine(r, col, indent);
    }
    return r;
}

void
reformat(istream& is)
{
    while (is && !is.eof()){
        string line;
        getline(is, line);
        if (line.size())
            cout << reformat(line) << endl;
    }
}

int 
main(int argc, char** argv)
{
    if (argc == 1)
        reformat(cin);
    else{
        ifstream is(argv[1]);
        if (!is || !is.is_open()){
            cout << "could not open " << argv[1] << endl;
            return -2;
        }
        reformat(is);
    }
    
    return 0;
}
