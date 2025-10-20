#include "CHTLJS/include/nodes/AstNode.h"

namespace CHTLJS
{
    std::unique_ptr<AstNode> ProgramNode::clone() const
    {
        auto node = std::make_unique<ProgramNode>();
        for (const auto& child : children)
        {
            node->children.push_back(child->clone());
        }
        return node;
    }
}
