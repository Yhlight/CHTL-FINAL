#pragma once

#include "../CHTLNode/Node.h"
#include "../CHTLNode/Statement.h"
#include <string>

namespace CHTL {

    class Evaluator; // Forward declaration

    class Generator {
    public:
        std::string Generate(const Program* program);

    private:
        std::string GenerateStatement(const Statement* stmt, Evaluator& evaluator);
        std::string GenerateElementStatement(const ElementStatement* stmt, Evaluator& evaluator);
        std::string GenerateTextStatement(const TextStatement* stmt);
        std::string GenerateBlockStatement(const BlockStatement* stmt, Evaluator& evaluator);
    };

} // namespace CHTL
