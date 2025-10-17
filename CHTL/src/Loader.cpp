#include "Loader.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <stdexcept>

namespace CHTL
{

std::string Loader::ReadFile(const std::string& base_file_path, const std::string& import_path)
{
    // If base_file_path is empty (e.g., for the very first file), use the current working directory.
    // Otherwise, use the directory of the file currently being parsed as the base for relative paths.
    std::filesystem::path base_dir = base_file_path.empty()
        ? std::filesystem::current_path()
        : std::filesystem::path(base_file_path).parent_path();

    std::filesystem::path target_file_path = base_dir / import_path;

    // Normalize the path to resolve ".." etc.
    target_file_path = target_file_path.lexically_normal();

    if (!std::filesystem::exists(target_file_path))
    {
        throw std::runtime_error("Imported file not found: " + target_file_path.string());
    }

    std::ifstream file_stream(target_file_path);
    if (!file_stream.is_open())
    {
        throw std::runtime_error("Could not open imported file: " + target_file_path.string());
    }

    std::stringstream buffer;
    buffer << file_stream.rdbuf();
    return buffer.str();
}

} // namespace CHTL