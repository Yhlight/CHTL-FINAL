#ifndef CJMODSCANNER_H
#define CJMODSCANNER_H

#include "Arg.h"
#include <string>

class CJMODScanner {
public:
    static Arg scan(const Arg& args, const std::string& keyword);
};

#endif // CJMODSCANNER_H
