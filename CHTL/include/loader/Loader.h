#pragma once

#include <string>
#include <memory>
#include <utility>
#include "nodes/ProgramNode.h"
#include "CHTLJS/include/nodes/ProgramNode.h"

namespace CHTL
{

    class Loader
    {
    public:
        static std::string ReadFile(const std::string& base_file_path, const std::string& import_path);
        static std::pair<std::unique_ptr<ProgramNode>, std::unique_ptr<CHTLJS::ProgramNode>> LoadModule(const std::string& base_file_path, const std::string& import_path);
    };

}