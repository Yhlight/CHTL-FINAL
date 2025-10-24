#ifndef CHTL_TEST_UTILS_H
#define CHTL_TEST_UTILS_H

#include <string>

// This macro is defined in CMakeLists.txt
#define CHTL_TEST_SRCDIR CHTL_SOURCE_DIR

std::string readFile(const std::string& path);
std::string normalize(const std::string& s);

#endif // CHTL_TEST_UTILS_H
