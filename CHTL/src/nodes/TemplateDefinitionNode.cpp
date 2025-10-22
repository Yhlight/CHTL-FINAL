#include "TemplateDefinitionNode.h"
#include "StylePropertyNode.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "nodes/ExpressionNode.h"
#include "AstNode.h"
#include "Serialize.h"

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

    void TemplateDefinitionNode::serialize(std::ostream& os) const
    {
        int node_type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&node_type), sizeof(node_type));
        Serializer::serialize(type, os);
        Serializer::serialize(name, os);
        Serializer::serialize(inherited_templates, os);
        Serializer::serialize(variables, os);
        Serializer::serialize(properties, os);
        Serializer::serialize(body, os);
    }

    std::unique_ptr<TemplateDefinitionNode> TemplateDefinitionNode::deserialize(std::istream& is)
    {
        auto node = std::make_unique<TemplateDefinitionNode>();
        Serializer::deserialize(node->type, is);
        Serializer::deserialize(node->name, is);
        Serializer::deserialize(node->inherited_templates, is);
        Serializer::deserialize(node->variables, is);
        Serializer::deserialize(node->properties, is);
        Serializer::deserialize(node->body, is);
        return node;
    }
}