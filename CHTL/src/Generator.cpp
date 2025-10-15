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
                    style_ss << prop->name << ":" << result.num << result.unit << ";";
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

        if (style_node)
        {
            for (const auto& style_child : style_node->children)
            {
                if (style_child->GetType() == NodeType::StyleRule)
                {
                    auto* rule = static_cast<const StyleRuleNode*>(style_child.get());
                    m_styleRules.push_back(rule);

                    // Add class to attributes
                    if (rule->selector[0] == '.')
                    {
                        final_attributes.push_back({"class", rule->selector.substr(1)});
                    }
                }
                else if (style_child->GetType() == NodeType::StyleProperty)
                {
                    if (has_inline_style) {
                        inline_styles << ";";
                    }
                    auto* prop = static_cast<StyleProperty*>(style_child.get());
                    Evaluator evaluator;
                    Value result = evaluator.Eval(prop->value.get());
                    inline_styles << prop->name << ":" << result.num << result.unit;
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
