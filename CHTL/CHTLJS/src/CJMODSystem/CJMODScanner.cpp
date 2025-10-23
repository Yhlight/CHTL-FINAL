#include "CJMODScanner.h"
#include <iostream>
#include <regex>

Arg CJMODScanner::scan(const Arg& args, const std::string& keyword, const std::string& source_code) {
    // This is a more realistic, yet still simplified, implementation of the scanner.
    // It simulates scanning a `console.log` statement and extracting its arguments.
    std::cout << "Scanning for keyword: " << keyword << std::endl;

    Arg result;

    if (keyword == "**") {
        std::regex re("\\((\\d+)\\s*\\*\\*\\s*(\\d+)\\)");
        std::smatch match;
        if (std::regex_search(source_code, match, re)) {
            result.add(AtomArg(match[1].str()));
            result.add(AtomArg("**"));
            result.add(AtomArg(match[2].str()));
        }
    }

    return result;
}
