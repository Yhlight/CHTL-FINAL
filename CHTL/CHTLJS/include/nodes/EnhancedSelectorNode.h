#pragma once

#include "CHTLJS/include/nodes/AstNode.h"
#include <string>

namespace CHTLJS
{
    struct EnhancedSelectorNode : public AstNode
    {
        std::string selector;

        EnhancedSelectorNode() = default;
        EnhancedSelectorNode(const std::string& selector);

        NodeType GetType() const override { return NodeType::EnhancedSelector; }
        std::string ToString() const override;
        std::unique_ptr<AstNode> clone() const override;
    };

} // namespace CHTLJS
