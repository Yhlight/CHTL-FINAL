#ifndef CHTL_COMPILER_H
#define CHTL_COMPILER_H

#include "AstNode.h"
#include <string>
#include <memory>

namespace CHTL
{

    class Compiler
    {
    public:
        Compiler() = default;

        std::unique_ptr<ProgramNode> Compile(const std::string& filepath);

    private:
        // Future private members can go here, e.g., for managing state or errors.
    };

}

#endif // CHTL_COMPILER_H
