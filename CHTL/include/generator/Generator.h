#pragma once

#include "AstNode.h"
#include "eval/Evaluator.h" // For EvalContext
#include "bridge/SaltBridge.h"
#include <string>
#include <sstream>
#include <memory>

// Forward declarations for CHTLJS nodes to avoid including the headers here
namespace CHTLJS {
    struct AstNode;
    struct ProgramNode;
    struct EnhancedSelectorNode;
    struct RawJSNode;
}

namespace CHTL
{
    /**
     * @class Generator
     * @brief The Generator class is responsible for traversing the AST and generating the final HTML output.
     *
     * It uses a visitor pattern to walk the AST, and for each node type, it generates
     * the corresponding HTML, CSS, and JavaScript. It also manages global styles and
     * communicates with the CHTL JS generator via the SaltBridge.
     */
    class Generator
    {
    public:
        /**
         * @brief Constructs a Generator.
         * @param bridge A shared pointer to the SaltBridge for communication with the CHTL JS compiler.
         */
        Generator(std::shared_ptr<SaltBridge> bridge);

        /**
         * @brief Generates the final HTML from the given AST.
         * @param program The root of the AST (a ProgramNode).
         * @return A string containing the generated HTML, including inlined styles and scripts.
         */
        std::string Generate(ProgramNode* program);

    private:
        // CHTL AST visitors
        void visit(AstNode* node, EvalContext& context);
        void visit(ProgramNode* node, EvalContext& context);
        void visit(ElementNode* node, EvalContext& context);
        void visit(TextNode* node, EvalContext& context);
        void visit(CommentNode* node, EvalContext& context);
        void visit(ScriptNode* node, EvalContext& context);
        void visit(OriginNode* node, EvalContext& context);
        void visit(NamespaceNode* node, EvalContext& context);
        void visit(TemplateUsageNode* node, EvalContext& context);
        void visit(CustomUsageNode* node, EvalContext& context);
        void visit(CustomDefinitionNode* node, EvalContext& context);
        void visit(ImportNode* node, EvalContext& context);
        void applyStyleTemplate(const TemplateDefinitionNode* tmpl, EvalContext& context, std::map<std::string, std::string>& property_map);
        void visit(StyleNode* node, EvalContext& context, ElementNode* parent);
        void visit(StyleRuleNode* node, EvalContext& context, ElementNode* parent);
        void visit(StyleProperty* node, EvalContext& context, std::stringstream& style_stream);
        void visit(IfNode* node, EvalContext& context);
        void visit(ElseNode* node, EvalContext& context);

        // CHTL JS AST visitors
        void visit(CHTLJS::AstNode* node, EvalContext& context);
        void visit(CHTLJS::ProgramNode* node, EvalContext& context);
        void visit(CHTLJS::EnhancedSelectorNode* node, EvalContext& context);
        void visit(CHTLJS::RawJSNode* node, EvalContext& context);


        std::stringstream m_output;
        std::stringstream m_global_styles;
        const ProgramNode* m_programNode = nullptr;
        std::shared_ptr<SaltBridge> m_bridge;
    };
}
