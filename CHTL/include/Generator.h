#pragma once

#include "AstNode.h"
#include "Evaluator.h" // For EvalContext
#include <string>
#include <sstream>

namespace CHTL
{
    class Generator
    {
    public:
        std::string Generate(ProgramNode* program);

    private:
        void visit(AstNode* node, EvalContext& context);
        void visit(ProgramNode* node, EvalContext& context);
        void visit(ElementNode* node, EvalContext& context);
        void visit(TextNode* node, EvalContext& context);
        void visit(CommentNode* node, EvalContext& context);
        void visit(NamespaceNode* node, EvalContext& context);
        void visit(TemplateUsageNode* node, EvalContext& context);
        void visit(CustomDefinitionNode* node, EvalContext& context);

        std::stringstream m_output;
        std::vector<const StyleRuleNode*> m_styleRules;
        const ProgramNode* m_programNode = nullptr;
    };
}
