#include "FileSystem.h"
#include <fstream>
#include <sstream>
#include <sys/stat.h>

namespace CHTL {

std::string FileSystem::readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        // In a real application, more robust error handling would be needed.
        // For now, returning an empty string indicates failure.
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void FileSystem::writeFile(const std::string& filePath, const std::string& content) {
    std::ofstream file(filePath);
    if (file.is_open()) {
        file << content;
    }
    // Again, more robust error handling would be beneficial here.
}

bool FileSystem::fileExists(const std::string& filePath) {
    struct stat buffer;
    return (stat(filePath.c_str(), &buffer) == 0);
}

} // namespace CHTL