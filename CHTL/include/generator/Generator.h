#pragma once

#include "AstNode.h"
#include "nodes/UseNode.h"
#include "Config.h"
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
    // Forward-declare the function that needs to be a friend
    class Generator;
    void evaluateAndStoreProperty(Generator* generator, StyleProperty* prop, EvalContext& context, std::map<std::string, std::string>& property_map);
    void renderContent(Generator* generator, const std::vector<std::unique_ptr<AstNode>>& nodes, EvalContext& context, const std::vector<const ExceptNode*>& except_nodes);

    class Generator
    {
    public:
        Generator(std::shared_ptr<SaltBridge> bridge);
        std::string Generate(ProgramNode* program);

        // Grant friendship
        friend void evaluateAndStoreProperty(Generator* generator, StyleProperty* prop, EvalContext& context, std::map<std::string, std::string>& property_map);
        friend void renderContent(Generator* generator, const std::vector<std::unique_ptr<AstNode>>& nodes, EvalContext& context, const std::vector<const ExceptNode*>& except_nodes);

    private:
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
        void visit(ExceptNode* node, EvalContext& context);
        void visit(ConfigurationNode* node, EvalContext& context);
        void visit(UseNode* node, EvalContext& context);
        void visit(ReactiveValueNode* node, EvalContext& context, std::stringstream& stream);

        // CHTL JS AST visitors
        void visit(CHTLJS::AstNode* node, EvalContext& context);
        void visit(CHTLJS::ProgramNode* node, EvalContext& context);
        void visit(CHTLJS::EnhancedSelectorNode* node, EvalContext& context);
        void visit(CHTLJS::RawJSNode* node, EvalContext& context);


        struct DynamicCondition {
            std::string condition;
            std::string target_id;
            std::string property_name;
            std::string property_value;
        };

        std::stringstream m_output;
        std::stringstream m_global_styles;
        const ProgramNode* m_programNode = nullptr;
        std::shared_ptr<SaltBridge> m_bridge;
        Config m_config;
        bool m_use_html5_doctype = false;
        bool m_has_reactive_values = false;
        std::vector<DynamicCondition> m_dynamic_conditions;
        int m_dynamic_id_counter = 0;
    };
}
