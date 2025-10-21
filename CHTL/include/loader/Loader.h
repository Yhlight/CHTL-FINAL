#pragma once

#include <string>
#include <memory>

namespace CHTL
{
    struct ProgramNode;

    class Loader
    {
    public:
        // Given the path of the file currently being processed (base_file_path)
        // and the path specified in an import statement (import_path),
        // this function resolves the full path and returns the content of the imported file.
        // It throws std::runtime_error if the file cannot be found or opened.
        static std::string ReadFile(const std::string& base_file_path, const std::string& import_path);
        static std::unique_ptr<ProgramNode> LoadModule(const std::string& base_file_path, const std::string& import_path);
    };

} // namespace CHTL