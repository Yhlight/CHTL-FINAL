#include "Generator.h"
#include "Evaluator.h"
#include <stdexcept>
#include <vector>
#include <unordered_set>
#include <map>

namespace CHTL
{
    std::string Generator::Generate(ProgramNode* program)
    {
        m_programNode = program;
        m_output.clear();
        m_output.str("");
        m_styleRules.clear();

        visit(program);

        std::string content = m_output.str();
        if (!m_styleRules.empty())
        {
            std::stringstream style_ss;
            style_ss << "<head><style>";
            for (const auto* rule : m_styleRules)
            {
                style_ss << rule->selector << "{";
                EvalContext context;
                context.program = m_programNode;
                for (const auto& prop : rule->properties)
                {
                    Evaluator evaluator;
                    Value result = evaluator.Eval(prop->value.get(), context);
                    context.values[prop->name] = result;
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

    void Generator::visit(AstNode* node)
    {
        if (!node) return;

        switch (node->GetType())
        {
            case NodeType::Program:
                visit(static_cast<ProgramNode*>(node));
                break;
            case NodeType::Element:
                visit(static_cast<ElementNode*>(node));
                break;
            case NodeType::Text:
                visit(static_cast<TextNode*>(node));
                break;
            case NodeType::Style:
                // Style nodes are handled by their parent ElementNode, so we do nothing here.
                break;
            case NodeType::Comment:
                visit(static_cast<CommentNode*>(node));
                break;
            case NodeType::TemplateDefinition:
                // Template definitions are collected by the parser and are not directly generated.
                break;
            case NodeType::TemplateUsage:
                visit(static_cast<TemplateUsageNode*>(node));
                break;
            case NodeType::CustomDefinition:
                // Custom definitions are not directly generated.
                break;
            case NodeType::CustomUsage:
                // CustomUsage nodes are handled by their parent ElementNode,
                // specifically for @Style usages. Direct generation is not applicable.
                break;
            default:
                throw std::runtime_error("Unknown AST node type in Generator");
        }
    }

    void Generator::visit(ProgramNode* node)
    {
        for (const auto& child : node->children)
        {
            visit(child.get());
        }
    }

    void Generator::visit(ElementNode* node)
    {
        // A temporary vector to hold attributes, allowing us to add automatic ones.
        std::vector<Attribute> final_attributes = node->attributes;

        // Find and process StyleNode to extract rules and inline styles
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
            EvalContext context;
            context.program = m_programNode;
            Evaluator evaluator;

            // Find main selector first
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

            // Process all style children in order, letting later ones override earlier ones in the map
            for (const auto& style_child : style_node->children)
            {
                if (style_child->GetType() == NodeType::StyleRule)
                {
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
                    if (m_programNode->templates.count(usage->name))
                    {
                        const auto* tmpl = m_programNode->templates.at(usage->name);
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
                    if (m_programNode->customs.count(usage->name))
                    {
                        const auto* custom_def = m_programNode->customs.at(usage->name);
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

        // Generate opening tag with all attributes
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

        // Visit all non-style children
        for (const auto& child : node->children)
        {
            if (child->GetType() != NodeType::Style)
            {
                visit(child.get());
            }
        }

        m_output << "</" << node->tag_name << ">";
    }

    void Generator::visit(TextNode* node)
    {
        m_output << node->value;
    }

    void Generator::visit(CommentNode* node)
    {
        m_output << "<!-- " << node->value << " -->";
    }

    void Generator::visit(TemplateUsageNode* node)
    {
        if (node->type == "@Element")
        {
            if (m_programNode->templates.count(node->name))
            {
                const auto* tmpl = m_programNode->templates.at(node->name);
                for (const auto& child : tmpl->body)
                {
                    visit(child.get());
                }
            }
        }
        // Note: @Style and @Var usages are handled inside visit(ElementNode) and the Evaluator,
        // so we don't need to handle them here. A direct call to visit a @Style usage
        // would be out of context.
    }

    void Generator::visit(CustomDefinitionNode* node)
    {
        // Custom definitions are not directly rendered, they are used by CustomUsageNodes.
        // So this function is intentionally empty.
    }
}
