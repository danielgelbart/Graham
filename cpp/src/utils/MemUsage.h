#ifndef MEM_USAGE_H
#define MEM_USAGE_H

#include <string>

void processMemUsage(double& vm_usage, double& resident_set);
void printMemUsage(const std::string& tag);


#endif //MEM_USAGE_H
