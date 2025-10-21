#include "NamespaceNode.h"

namespace CHTL
{
    std::string NamespaceNode::ToString() const
    {
        return "[Namespace] " + name + " { ... }";
    }

    std::unique_ptr<AstNode> NamespaceNode::clone() const
    {
        auto node = std::make_unique<NamespaceNode>();
        node->name = this->name;
        for (const auto &child : this->children)
        {
            node->children.push_back(child->clone());
        }
        return node;
    }
}