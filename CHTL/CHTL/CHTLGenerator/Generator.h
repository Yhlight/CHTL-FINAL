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
    class TemplateDefinitionStatement; // Forward declaration
    using ElementMap = std::map<std::string, const ElementStatement*>;

    class Generator {
    public:
        std::string Generate(const Program* program);

    private:
        void CollectStyleTemplates(const Node* node);
        void CollectElementTemplates(const Node* node);
        void CollectStyleRules(const Node* node, const ElementStatement* parent);
        std::string RenderGlobalStyles(Evaluator& evaluator, std::map<std::string, std::unique_ptr<Object>>& context, const ElementMap& docMap);

        std::string GenerateStatement(const Statement* stmt, Evaluator& evaluator, std::map<std::string, std::unique_ptr<Object>>& context, const ElementMap& docMap);
        std::string GenerateElementStatement(const ElementStatement* stmt, Evaluator& evaluator, std::map<std::string, std::unique_ptr<Object>>& context, const ElementMap& docMap);
        std::string GenerateTextStatement(const TextStatement* stmt);
        std::string GenerateBlockStatement(const BlockStatement* stmt, Evaluator& evaluator, std::map<std::string, std::unique_ptr<Object>>& context, const ElementMap& docMap);

        std::vector<std::pair<const StyleRule*, const ElementStatement*>> global_styles;
        std::map<std::string, const TemplateDefinitionStatement*> style_templates;
        std::map<std::string, const TemplateDefinitionStatement*> element_templates;
    };

} // namespace CHTL
