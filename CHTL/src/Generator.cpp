#include "Generator.h"
#include "Evaluator.h" // EvalContext is defined here
#include <stdexcept>
#include <vector>
#include <unordered_set>
#include <map>

namespace CHTL
{
    // Forward declare the global namespace constant
    extern const std::string GLOBAL_NAMESPACE;

    std::string Generator::Generate(ProgramNode* program)
    {
        m_programNode = program;
        m_output.clear();
        m_output.str("");
        m_styleRules.clear();

        EvalContext context;
        context.program = program;
        context.current_namespace = GLOBAL_NAMESPACE;

        visit(program, context);

        std::string content = m_output.str();
        if (!m_styleRules.empty())
        {
            std::stringstream style_ss;
            style_ss << "<head><style>";
            for (const auto* rule : m_styleRules)
            {
                style_ss << rule->selector << "{";
                // Note: This context for style rules is basic and doesn't know about namespaces.
                // This could be a future improvement.
                EvalContext rule_context;
                rule_context.program = m_programNode;
                for (const auto& prop : rule->properties)
                {
                    Evaluator evaluator;
                    Value result = evaluator.Eval(prop->value.get(), rule_context);
                    rule_context.values[prop->name] = result;
                    style_ss << prop->name << ":";
                    if (result.type == ValueType::STRING) {
                        style_ss << result.str;
                    } else {
                        style_ss << result.num << result.unit;
                    }
                    style_ss << ";";
                }
                style_ss << "}";
            }
            style_ss << "</style></head>";
            return style_ss.str() + content;
        }

        return content;
    }

    void Generator::visit(AstNode* node, EvalContext& context)
    {
        if (!node) return;

        switch (node->GetType())
        {
            case NodeType::Program:
                visit(static_cast<ProgramNode*>(node), context);
                break;
            case NodeType::Element:
                visit(static_cast<ElementNode*>(node), context);
                break;
            case NodeType::Text:
                visit(static_cast<TextNode*>(node), context);
                break;
            case NodeType::Namespace:
                visit(static_cast<NamespaceNode*>(node), context);
                break;
            case NodeType::Comment:
                visit(static_cast<CommentNode*>(node), context);
                break;
            case NodeType::TemplateUsage:
                visit(static_cast<TemplateUsageNode*>(node), context);
                break;
            case NodeType::TemplateDefinition:
            case NodeType::CustomDefinition:
            case NodeType::Style:
            case NodeType::CustomUsage:
                // These nodes are handled contextually by their parents.
                break;
            default:
                throw std::runtime_error("Unknown AST node type in Generator");
        }
    }

    void Generator::visit(ProgramNode* node, EvalContext& context)
    {
        for (const auto& child : node->children)
        {
            visit(child.get(), context);
        }
    }

    void Generator::visit(NamespaceNode* node, EvalContext& context)
    {
        std::string previous_namespace = context.current_namespace;
        context.current_namespace = node->name;
        for (const auto& child : node->children)
        {
            visit(child.get(), context);
        }
        context.current_namespace = previous_namespace;
    }


    void Generator::visit(ElementNode* node, EvalContext& context)
    {
        std::vector<Attribute> final_attributes = node->attributes;
        StyleNode* style_node = nullptr;
        for (const auto& child : node->children)
        {
            if (child->GetType() == NodeType::Style)
            {
                style_node = static_cast<StyleNode*>(child.get());
                break;
            }
        }

        std::map<std::string, std::string> inline_styles_map;
        std::string main_selector = "";

        if (style_node)
        {
            // The context passed in from the parent already has the correct namespace
            Evaluator evaluator;

            // First pass for main selector, same as before
            for (const auto& style_child : style_node->children)
            {
                if (style_child->GetType() == NodeType::StyleRule)
                {
                    auto* rule = static_cast<const StyleRuleNode*>(style_child.get());
                    if (main_selector.empty() && (rule->selector[0] == '.' || rule->selector[0] == '#'))
                    {
                        main_selector = rule->selector;
                    }
                }
            }

            for (const auto& style_child : style_node->children)
            {
                if (style_child->GetType() == NodeType::StyleRule) {
                    auto* rule = static_cast<StyleRuleNode*>(style_child.get());
                    if (rule->selector[0] == '&')
                    {
                        rule->selector.replace(0, 1, main_selector);
                    }
                    else if (rule->selector[0] == '.')
                    {
                        final_attributes.push_back({"class", rule->selector.substr(1)});
                    }
                    m_styleRules.push_back(rule);
                }
                else if (style_child->GetType() == NodeType::TemplateUsage)
                {
                    auto* usage = static_cast<TemplateUsageNode*>(style_child.get());
                    const TemplateDefinitionNode* tmpl = nullptr;
                    // 1. Look in current namespace
                    if (m_programNode->templates.count(context.current_namespace) && m_programNode->templates.at(context.current_namespace).count(usage->name)) {
                        tmpl = m_programNode->templates.at(context.current_namespace).at(usage->name);
                    }
                    // 2. Look in global namespace (if not already there)
                    else if (context.current_namespace != GLOBAL_NAMESPACE && m_programNode->templates.count(GLOBAL_NAMESPACE) && m_programNode->templates.at(GLOBAL_NAMESPACE).count(usage->name)) {
                        tmpl = m_programNode->templates.at(GLOBAL_NAMESPACE).at(usage->name);
                    }

                    if (tmpl) {
                        for (const auto& prop_ptr : tmpl->properties)
                        {
                            Value result = evaluator.Eval(prop_ptr->value.get(), context);
                            context.values[prop_ptr->name] = result;
                            std::stringstream ss;
                            if (result.type == ValueType::STRING) ss << result.str;
                            else ss << result.num << result.unit;
                            inline_styles_map[prop_ptr->name] = ss.str();
                        }
                    }
                }
                else if (style_child->GetType() == NodeType::CustomUsage)
                {
                     auto* usage = static_cast<CustomUsageNode*>(style_child.get());
                     const CustomDefinitionNode* custom_def = nullptr;
                     // 1. Look in current namespace
                     if (m_programNode->customs.count(context.current_namespace) && m_programNode->customs.at(context.current_namespace).count(usage->name)) {
                         custom_def = m_programNode->customs.at(context.current_namespace).at(usage->name);
                     }
                     // 2. Look in global namespace
                     else if (context.current_namespace != GLOBAL_NAMESPACE && m_programNode->customs.count(GLOBAL_NAMESPACE) && m_programNode->customs.at(GLOBAL_NAMESPACE).count(usage->name)) {
                         custom_def = m_programNode->customs.at(GLOBAL_NAMESPACE).at(usage->name);
                     }

                    if (custom_def) {
                        std::unordered_set<std::string> deleted_properties;
                        for (const auto& spec : usage->specializations)
                        {
                            if (spec->GetType() == NodeType::DeleteSpecialization)
                            {
                                deleted_properties.insert(static_cast<DeleteSpecializationNode*>(spec.get())->property_name);
                            }
                        }

                        for (const auto& child_node : custom_def->children)
                        {
                            if (child_node->GetType() == NodeType::StyleProperty)
                            {
                                auto* prop_ptr = static_cast<StyleProperty*>(child_node.get());
                                if (deleted_properties.find(prop_ptr->name) == deleted_properties.end())
                                {
                                    Value result = evaluator.Eval(prop_ptr->value.get(), context);
                                    context.values[prop_ptr->name] = result;
                                    std::stringstream ss;
                                    if (result.type == ValueType::STRING) ss << result.str;
                                    else ss << result.num << result.unit;
                                    inline_styles_map[prop_ptr->name] = ss.str();
                                }
                            }
                        }
                    }
                }
                else if (style_child->GetType() == NodeType::StyleProperty)
                {
                    auto* prop = static_cast<StyleProperty*>(style_child.get());
                    Value result = evaluator.Eval(prop->value.get(), context);
                    context.values[prop->name] = result;
                    std::stringstream ss;
                    if (result.type == ValueType::STRING) ss << result.str;
                    else ss << result.num << result.unit;
                    inline_styles_map[prop->name] = ss.str();
                }
            }
        }

        m_output << "<" << node->tag_name;
        for (const auto& attr : final_attributes)
        {
            m_output << " " << attr.name << "=\"" << attr.value << "\"";
        }
        if (!inline_styles_map.empty())
        {
            m_output << " style=\"";
            for (auto it = inline_styles_map.begin(); it != inline_styles_map.end(); ++it)
            {
                m_output << it->first << ":" << it->second << ";";
            }
            m_output << "\"";
        }
        m_output << ">";

        for (const auto& child : node->children)
        {
            if (child->GetType() != NodeType::Style)
            {
                visit(child.get(), context);
            }
        }

        m_output << "</" << node->tag_name << ">";
    }

    void Generator::visit(TextNode* node, EvalContext& context)
    {
        m_output << node->value;
    }

    void Generator::visit(CommentNode* node, EvalContext& context)
    {
        m_output << "<!-- " << node->value << " -->";
    }

    void Generator::visit(TemplateUsageNode* node, EvalContext& context)
    {
        if (node->type == "@Element")
        {
            const TemplateDefinitionNode* tmpl = nullptr;
             // 1. Look in current namespace
            if (m_programNode->templates.count(context.current_namespace) && m_programNode->templates.at(context.current_namespace).count(node->name)) {
                tmpl = m_programNode->templates.at(context.current_namespace).at(node->name);
            }
            // 2. Look in global namespace
            else if (context.current_namespace != GLOBAL_NAMESPACE && m_programNode->templates.count(GLOBAL_NAMESPACE) && m_programNode->templates.at(GLOBAL_NAMESPACE).count(node->name)) {
                tmpl = m_programNode->templates.at(GLOBAL_NAMESPACE).at(node->name);
            }

            if (tmpl)
            {
                for (const auto& child : tmpl->body)
                {
                    visit(child.get(), context);
                }
            }
        }
    }

    void Generator::visit(CustomDefinitionNode* node, EvalContext& context)
    {
        // Intentionally empty.
    }
}
