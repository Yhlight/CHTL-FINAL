#ifndef CJMODSCANNER_H
#define CJMODSCANNER_H

#include "Arg.h"
#include <string>

/**
 * @class CJMODScanner
 * @brief Scans for specific syntax patterns in the source code based on a keyword.
 *
 * This class is intended to be a flexible and extensible way to find and extract
 * CHTL JS syntax fragments.
 */
class CJMODScanner {
public:
    /**
     * @brief Scans the source code for a given syntax pattern.
     * @param args The parsed argument list that defines the syntax to be scanned.
     * @param keyword The keyword that triggers the scan.
     * @param source_code The source code to be scanned.
     * @return An Arg object containing the values extracted from the source code.
     */
    static Arg scan(const Arg& args, const std::string& keyword, const std::string& source_code);
};

#endif // CJMODSCANNER_H
