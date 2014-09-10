#ifndef MAIN_H
#define MAIN_H

#include <string>

class RunInfo {
public:
    RunInfo(const std::string basePath) {
        _basePath = basePath;
        _this = this;
    }

    static RunInfo* instance() { return _this; }
    std::string basePath() const { return _basePath; } 

private:  
    static const std::string _basePath;
    static const RunInfo* _this;
};

#endif //MAIN_H
