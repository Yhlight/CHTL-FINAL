#include "CustomUsageNode.h"

namespace CHTL
{
    std::string CustomUsageNode::ToString() const
    {
        return "CustomUsage(" + type + " " + name + ")";
    }

    std::unique_ptr<AstNode> CustomUsageNode::clone() const
    {
        auto node = std::make_unique<CustomUsageNode>();
        node->type = this->type;
        node->name = this->name;
        for (const auto &child : this->specializations)
        {
            node->specializations.push_back(child->clone());
        }
        return node;
    }
}