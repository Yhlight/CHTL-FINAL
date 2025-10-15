#include "Generator.h"
#include "Evaluator.h"
#include <stdexcept>
#include <vector>

namespace CHTL
{
    std::string Generator::Generate(ProgramNode* program)
    {
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
                for (const auto& prop : rule->properties)
                {
                    Evaluator evaluator;
                    Value result = evaluator.Eval(prop->value.get());
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

        std::stringstream inline_styles;
        bool has_inline_style = false;
        std::string main_selector = "";

        if (style_node)
        {
            // First pass: find main selector and inline styles
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

            // Second pass: process all rules
            for (const auto& style_child : style_node->children)
            {
                if (style_child->GetType() == NodeType::StyleRule)
                {
                    auto* rule = static_cast<StyleRuleNode*>(style_child.get());

                    bool is_contextual = (rule->selector[0] == '&');

                    if (is_contextual)
                    {
                        rule->selector.replace(0, 1, main_selector);
                    }
                    else if (rule->selector[0] == '.')
                    {
                        final_attributes.push_back({"class", rule->selector.substr(1)});
                    }

                    m_styleRules.push_back(rule);
                }
                else if (style_child->GetType() == NodeType::StyleProperty)
                {
                    if (has_inline_style) {
                        inline_styles << ";";
                    }
                    auto* prop = static_cast<StyleProperty*>(style_child.get());
                    Evaluator evaluator;
                    Value result = evaluator.Eval(prop->value.get());
                    inline_styles << prop->name << ":";
                    if (result.type == ValueType::STRING) {
                        inline_styles << result.str;
                    } else {
                        inline_styles << result.num << result.unit;
                    }
                    has_inline_style = true;
                }
            }
        }

        // Generate opening tag with all attributes
        m_output << "<" << node->tag_name;
        for (const auto& attr : final_attributes)
        {
            m_output << " " << attr.name << "=\"" << attr.value << "\"";
        }

        if (has_inline_style)
        {
            m_output << " style=\"" << inline_styles.str() << ";\"";
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
}
