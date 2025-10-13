#pragma once

#include "../CHTLNode/Node.h"
#include "../CHTLNode/Statement.h"
#include "../CHTLNode/Style.h"
#include <string>

namespace CHTL {

    class Evaluator; // Forward declaration

    class Generator {
    public:
        std::string Generate(const Program* program);

    private:
        void CollectStyleRules(const Node* node, const ElementStatement* parent);
        std::string RenderGlobalStyles(Evaluator& evaluator);

        std::string GenerateStatement(const Statement* stmt, Evaluator& evaluator);
        std::string GenerateElementStatement(const ElementStatement* stmt, Evaluator& evaluator);
        std::string GenerateTextStatement(const TextStatement* stmt);
        std::string GenerateBlockStatement(const BlockStatement* stmt, Evaluator& evaluator);

        std::vector<std::pair<const StyleRule*, const ElementStatement*>> global_styles;
    };

} // namespace CHTL
