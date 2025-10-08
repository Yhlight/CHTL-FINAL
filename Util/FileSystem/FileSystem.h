#ifndef CHTL_FILESYSTEM_H
#define CHTL_FILESYSTEM_H

#include <string>
#include <vector>

namespace CHTL {

class FileSystem {
public:
    // Reads the entire content of a file into a string
    static std::string readFile(const std::string& filePath);

    // Writes a string to a file
    static void writeFile(const std::string& filePath, const std::string& content);

    // Checks if a file exists
    static bool fileExists(const std::string& filePath);
};

} // namespace CHTL

#endif // CHTL_FILESYSTEM_H