#include "AstNode.h"
#include <sstream>

namespace CHTL
{
    std::string ElementNode::ToString() const
    {
        std::stringstream ss;
        ss << "ElementNode(tag: " << tag_name << ", attributes: [";
        for (size_t i = 0; i < attributes.size(); ++i)
        {
            ss << attributes[i].name << ": \"" << attributes[i].value << "\"";
            if (i < attributes.size() - 1)
            {
                ss << ", ";
            }
        }
        ss << "], children: [";
        for (size_t i = 0; i < children.size(); ++i)
        {
            ss << children[i]->ToString();
            if (i < children.size() - 1)
            {
                ss << ", ";
            }
        }
        ss << "])";
        return ss.str();
    }

    std::string StyleNode::ToString() const
    {
        std::stringstream ss;
        ss << "StyleNode(children: [";
        for (size_t i = 0; i < children.size(); ++i)
        {
            ss << children[i]->ToString();
            if (i < children.size() - 1)
            {
                ss << ", ";
            }
        }
        ss << "])";
        return ss.str();
    }

    std::string StyleProperty::ToString() const
    {
        return "StyleProperty(" + name + ": " + value->ToString() + ")";
    }

    std::string StyleRuleNode::ToString() const
    {
        std::stringstream ss;
        ss << "StyleRuleNode(selector: " << selector << ", properties: [";
        for (size_t i = 0; i < properties.size(); ++i)
        {
            ss << properties[i]->ToString();
            if (i < properties.size() - 1)
            {
                ss << ", ";
            }
        }
        ss << "])";
        return ss.str();
    }

    std::string ConditionalExpression::ToString() const
    {
        std::stringstream ss;
        ss << "(" << condition->ToString() << " ? " << consequence->ToString() << " : " << alternative->ToString() << ")";
        return ss.str();
    }

    std::string ProgramNode::ToString() const
    {
        std::stringstream ss;
        ss << "ProgramNode([";
        for (size_t i = 0; i < children.size(); ++i)
        {
            ss << children[i]->ToString();
            if (i < children.size() - 1)
            {
                ss << ", ";
            }
        }
        ss << "])";
        return ss.str();
    }

    std::string TemplateUsageNode::ToString() const
    {
        return "TemplateUsage(" + type + " " + name + ")";
    }

    std::string TemplateDefinitionNode::ToString() const
    {
        std::stringstream ss;
        ss << "TemplateDefinition(" << type << " " << name << " { ";
        for (size_t i = 0; i < properties.size(); ++i)
        {
            ss << properties[i]->ToString();
            if (i < properties.size() - 1)
            {
                ss << ", ";
            }
        }
        ss << " })";
        return ss.str();
    }

    std::string VariableAccessNode::ToString() const
    {
        return template_name + "(" + variable_name + ")";
    }
}
