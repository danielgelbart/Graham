#ifndef INFO_H
#define INFO_H

enum class StatementType {K10,Q10};

struct Info{
    string ticker;
    size_t year;
    StatementType type;
    size_t quarter; // if 10-k, then quaeter==0
};

#endif
