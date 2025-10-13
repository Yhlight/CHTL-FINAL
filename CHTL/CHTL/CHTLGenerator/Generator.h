#pragma once

#include "../CHTLNode/Node.h"
#include "../CHTLNode/Statement.h"
#include <string>

namespace CHTL {

    class Generator {
    public:
        std::string Generate(const Program* program);

    private:
        std::string GenerateStatement(const Statement* stmt);
        std::string GenerateElementStatement(const ElementStatement* stmt);
        std::string GenerateTextStatement(const TextStatement* stmt);
        std::string GenerateBlockStatement(const BlockStatement* stmt);
    };

} // namespace CHTL
