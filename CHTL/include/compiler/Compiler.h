#ifndef CHTL_COMPILER_H
#define CHTL_COMPILER_H

#include "AstNode.h"
#include <string>
#include <memory>
#include <vector>

namespace CHTL
{

    class Compiler
    {
    public:
        Compiler() = default;

        void Reset();
        std::unique_ptr<ProgramNode> Compile(const std::string& filepath);
        const std::vector<std::string>& GetErrors() const { return m_errors; }

    private:
        std::vector<std::string> m_errors;
    };

}

#endif // CHTL_COMPILER_H
