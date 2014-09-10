#ifndef RUN_INFO_H
#define RUN_INFO_H

#include <string>
#include "dmmm_identifiers.hpp"

class RunInfo {
public:
    RunInfo(const std::string basePath) {
        _basePath = basePath;
        _this = this;
    }

    static RunInfo* instance() { return _this; }
    std::string basePath() const { return _basePath; } 
    std::string mirrorsDir() const;
    //std::string mirrorDir(const DMMM::I_Account accountId) const;
    std::string sitemapsDir() const;

private:  
    std::string _basePath;
    static RunInfo* _this;
};

#endif //RUN_INFO_H
