#include "loader/Loader.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "nodes/ProgramNode.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <stdexcept>
#include <memory>

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

std::unique_ptr<ProgramNode> Loader::LoadModule(const std::string& base_file_path, const std::string& import_path)
{
    std::filesystem::path base_dir = base_file_path.empty()
        ? std::filesystem::current_path()
        : std::filesystem::path(base_file_path).parent_path();
    std::filesystem::path target_file_path = (base_dir / import_path).lexically_normal();

    std::string extension = target_file_path.extension().string();

    if (extension == ".cmod" || extension == ".cjmod")
    {
        // Placeholder: In a real implementation, this would involve
        // reading the binary file and deserializing it into a ProgramNode.
        auto program = std::make_unique<ProgramNode>();
        // Populate program with deserialized data...
        return program;
    }
    else
    {
        std::string file_content = ReadFile(base_file_path, import_path);
        Lexer lexer(file_content);
        Parser parser(lexer, target_file_path.string());
        return parser.ParseProgram();
    }
}

} // namespace CHTL