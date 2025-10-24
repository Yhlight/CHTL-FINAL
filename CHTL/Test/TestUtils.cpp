#include "TestUtils.h"
#include <fstream>
#include <sstream>
#include <algorithm>

std::string readFile(const std::string& path) {
    std::string full_path = std::string(CHTL_TEST_SRCDIR) + "/" + path;
    std::ifstream file(full_path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + full_path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string normalize(const std::string& s) {
    std::string out;
    std::copy_if(s.begin(), s.end(), std::back_inserter(out), [](char c){ return !isspace(c); });
    return out;
}
