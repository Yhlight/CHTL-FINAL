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
        visit(program);
        return m_output.str();
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
        m_output << "<" << node->tag_name;

        // Append attributes
        for (const auto& attr : node->attributes)
        {
            m_output << " " << attr.name << "=\"" << attr.value << "\"";
        }

        // Find and process StyleNode
        StyleNode* style_node = nullptr;
        for (const auto& child : node->children)
        {
            if (child->GetType() == NodeType::Style)
            {
                style_node = static_cast<StyleNode*>(child.get());
                break; // Assume only one style block per element
            }
        }

        if (style_node && !style_node->properties.empty())
        {
            Evaluator evaluator;
            m_output << " style=\"";
            for (size_t i = 0; i < style_node->properties.size(); ++i)
            {
                Value result = evaluator.Eval(style_node->properties[i].value.get());
                m_output << style_node->properties[i].name << ":" << result.num << result.unit;
                if (i < style_node->properties.size() - 1)
                {
                    m_output << ";";
                }
            }
            m_output << ";\"";
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
}
