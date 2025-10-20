#pragma once

#include "nodes/ProgramNode.h"
#include <string>
#include <memory>
#include <vector>
#include <unordered_set>

namespace CHTL
{
    class Compiler
    {
    public:
        Compiler();

        std::unique_ptr<ProgramNode> Compile(const std::string& file_path);

        const std::vector<std::string>& GetErrors() const { return m_errors; }

    private:
        // Scans a file and its config imports recursively.
        void ScanForConfigs(const std::string& file_path);

        std::vector<std::string> m_errors;
        std::unordered_set<std::string> m_scanned_files;
    };
}