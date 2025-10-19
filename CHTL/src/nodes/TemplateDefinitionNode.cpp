#include "TemplateDefinitionNode.h"
#include "StylePropertyNode.h"
#include <sstream>

namespace CHTL
{
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

    std::unique_ptr<AstNode> TemplateDefinitionNode::clone() const
    {
        auto node = std::make_unique<TemplateDefinitionNode>();
        node->type = this->type;
        node->name = this->name;
        node->inherited_templates = this->inherited_templates;
        for (const auto& var : variables)
        {
            node->variables[var.first] = std::unique_ptr<Expression>(static_cast<Expression*>(var.second->clone().release()));
        }
        for (const auto& prop : properties)
        {
            node->properties.push_back(prop->clone());
        }
        for (const auto& child : body)
        {
            node->body.push_back(child->clone());
        }
        return node;
    }
}