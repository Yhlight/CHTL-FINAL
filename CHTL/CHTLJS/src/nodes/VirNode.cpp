#include "CHTLJS/include/nodes/VirNode.h"

namespace CHTLJS
{
    std::unique_ptr<AstNode> VirNode::clone() const
    {
        auto node = std::make_unique<VirNode>();
        node->name = name;
        node->properties = properties;
        return node;
    }
}
