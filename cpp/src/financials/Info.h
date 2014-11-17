#ifndef INFO_H
#define INFO_H

enum class StatementType {K10,Q10};

struct Info{
    Info() {}
    Info(string& tik, size_t yy, StatementType tt, size_t qq=0): 
        ticker( tik ), year( yy ), type( tt ), quarter( qq ) {}

// members
    string ticker;
    size_t year;
    StatementType type;
    size_t quarter; // if 10-k, then quaeter==0
};

#endif
