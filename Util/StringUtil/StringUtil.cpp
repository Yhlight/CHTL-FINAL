#include "StringUtil.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

std::string StringUtil::trim(const std::string& str) {
    const std::string whitespace = " \t\n\r\f\v";
    size_t first = str.find_first_not_of(whitespace);
    if (std::string::npos == first) {
        return "";
    }
    size_t last = str.find_last_not_of(whitespace);
    return str.substr(first, (last - first + 1));
}

std::vector<std::string> StringUtil::split(const std::string& str, char delimiter) {
    if (str.empty()) {
        return {};
    }
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    if (!str.empty() && str.back() == delimiter) {
        tokens.push_back("");
    }
    return tokens;
}

bool StringUtil::startsWith(const std::string& str, const std::string& prefix) {
    if (str.length() < prefix.length()) {
        return false;
    }
    return str.rfind(prefix, 0) == 0;
}

bool StringUtil::endsWith(const std::string& str, const std::string& suffix) {
    if (str.length() < suffix.length()) {
        return false;
    }
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

} // namespace CHTL