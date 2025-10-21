#include "nodes/DeleteSpecializationNode.h"

namespace CHTL
{
    std::unique_ptr<AstNode> DeleteSpecializationNode::clone() const
    {
        auto node = std::make_unique<DeleteSpecializationNode>();
        node->property_name = this->property_name;
        return node;
    }
}
