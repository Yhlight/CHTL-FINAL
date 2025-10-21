#include "CHTLJS/include/nodes/EnhancedSelectorNode.h"

namespace CHTLJS
{
    std::string EnhancedSelectorNode::ToString() const
    {
        return "EnhancedSelectorNode(selector: \"" + selector + "\")";
    }

    std::unique_ptr<AstNode> EnhancedSelectorNode::clone() const
    {
        auto node = std::make_unique<EnhancedSelectorNode>();
        node->selector = this->selector;
        return node;
    }

} // namespace CHTLJS
