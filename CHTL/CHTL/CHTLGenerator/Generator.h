#pragma once

#include "../CHTLNode/Node.h"
#include "../CHTLNode/Statement.h"
#include "../CHTLNode/Style.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

    class Evaluator; // Forward declaration
    class Object;    // Forward declaration

    class Generator {
    public:
        std::string Generate(const Program* program);

    private:
        void CollectStyleRules(const Node* node, const ElementStatement* parent);
        std::string RenderGlobalStyles(Evaluator& evaluator, std::map<std::string, std::unique_ptr<Object>>& context);

        std::string GenerateStatement(const Statement* stmt, Evaluator& evaluator, std::map<std::string, std::unique_ptr<Object>>& context);
        std::string GenerateElementStatement(const ElementStatement* stmt, Evaluator& evaluator, std::map<std::string, std::unique_ptr<Object>>& context);
        std::string GenerateTextStatement(const TextStatement* stmt);
        std::string GenerateBlockStatement(const BlockStatement* stmt, Evaluator& evaluator, std::map<std::string, std::unique_ptr<Object>>& context);

        std::vector<std::pair<const StyleRule*, const ElementStatement*>> global_styles;
    };

} // namespace CHTL
