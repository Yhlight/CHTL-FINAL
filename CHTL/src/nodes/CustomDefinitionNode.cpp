#include "CustomDefinitionNode.h"

namespace CHTL
{
    std::string CustomDefinitionNode::ToString() const
    {
        return "CustomDefinition(" + type + " " + name + ")";
    }

    std::unique_ptr<AstNode> CustomDefinitionNode::clone() const
    {
        auto node = std::make_unique<CustomDefinitionNode>();
        node->type = this->type;
        node->name = this->name;
        for (const auto& child : children)
        {
            node->children.push_back(child->clone());
        }
        return node;
    }
}