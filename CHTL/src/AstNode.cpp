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

    std::string ExceptNode::ToString() const
    {
        std::stringstream ss;
        ss << "ExceptNode(constraints: [";
        for (size_t i = 0; i < constraints.size(); ++i)
        {
            ss << "{";
            for (size_t j = 0; j < constraints[i].path.size(); ++j)
            {
                ss << constraints[i].path[j];
                if (j < constraints[i].path.size() - 1) ss << " ";
            }
            ss << "}";
            if (i < constraints.size() - 1) ss << ", ";
        }
        ss << "])";
        return ss.str();
    }

    std::string InsertSpecializationNode::ToString() const
    {
        std::stringstream ss;
        ss << "InsertSpecializationNode(position: " << position;
        if (!target_selector.empty())
        {
            ss << ", target: " << target_selector;
        }
        ss << ", content: [";
        for (size_t i = 0; i < content.size(); ++i)
        {
            ss << content[i]->ToString();
            if (i < content.size() - 1)
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
    ss << "(" << condition->ToString() << " ? " << consequence->ToString();
    if (alternative)
    {
        ss << " : " << alternative->ToString();
    }
    ss << ")";
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
        if (!properties.empty())
        {
            for (size_t i = 0; i < properties.size(); ++i)
            {
                ss << properties[i]->ToString();
                if (i < properties.size() - 1)
                {
                    ss << ", ";
                }
            }
        }
        if (!body.empty())
        {
            for (size_t i = 0; i < body.size(); ++i)
            {
                ss << body[i]->ToString();
                if (i < body.size() - 1)
                {
                    ss << ", ";
                }
            }
        }
        ss << " })";
        return ss.str();
    }

    std::string VariableAccessNode::ToString() const
    {
        return template_name + "(" + variable_name + ")";
    }

    std::string CustomUsageNode::ToString() const
    {
        return "CustomUsage(" + type + " " + name + ")";
    }

    std::string CustomDefinitionNode::ToString() const
    {
        return "CustomDefinition(" + type + " " + name + ")";
    }

    std::string NamespaceNode::ToString() const
    {
        return "[Namespace] " + name + " { ... }";
    }

    std::string OriginNode::ToString() const
    {
        std::string result = "[Origin] " + type;
        if (!name.empty())
        {
            result += " " + name;
        }
        result += " { " + content + " }";
        return result;
    }

    std::string ExpressionListNode::ToString() const
    {
        std::stringstream ss;
        ss << "ExpressionList([";
        for (size_t i = 0; i < expressions.size(); ++i)
        {
            ss << expressions[i]->ToString();
            if (i < expressions.size() - 1)
            {
                ss << ", ";
            }
        }
        ss << "])";
        return ss.str();
    }

    // Note: DeleteSpecializationNode::ToString() is already defined inline in AstNode.h
    // No implementation needed here unless it's moved.

    std::string NameConfigNode::ToString() const
    {
        std::stringstream ss;
        ss << "NameConfigNode(settings: [";
        for (auto it = settings.begin(); it != settings.end(); ++it)
        {
            ss << it->first << ": " << it->second;
            if (std::next(it) != settings.end())
            {
                ss << ", ";
            }
        }
        ss << "])";
        return ss.str();
    }

    std::string ConfigurationNode::ToString() const
    {
        std::stringstream ss;
        ss << "ConfigurationNode(";
        if (!name.empty())
        {
            ss << "name: " << name << ", ";
        }
        ss << "settings: [";
        for (auto it = settings.begin(); it != settings.end(); ++it)
        {
            ss << it->first << ": " << it->second;
            if (std::next(it) != settings.end())
            {
                ss << ", ";
            }
        }
        ss << "]";
        if (name_config)
        {
            ss << ", name_config: " << name_config->ToString();
        }
        ss << ")";
        return ss.str();
    }

    std::string UseNode::ToString() const
    {
        std::stringstream ss;
        ss << "UseNode(path: [";
        for (size_t i = 0; i < path.size(); ++i)
        {
            ss << path[i];
            if (i < path.size() - 1)
            {
                ss << ", ";
            }
        }
        ss << "])";
        return ss.str();
    }
}
