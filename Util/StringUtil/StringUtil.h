#ifndef CHTL_STRINGUTIL_H
#define CHTL_STRINGUTIL_H

#include <string>
#include <vector>

namespace CHTL {

class StringUtil {
public:
    // Trims whitespace from both ends of a string
    static std::string trim(const std::string& str);

    // Splits a string by a delimiter
    static std::vector<std::string> split(const std::string& str, char delimiter);

    // Checks if a string starts with a given prefix
    static bool startsWith(const std::string& str, const std::string& prefix);

    // Checks if a string ends with a given suffix
    static bool endsWith(const std::string& str, const std::string& suffix);
};

} // namespace CHTL

#endif // CHTL_STRINGUTIL_H