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
        void serialize(std::ostream& os) const override;
        static std::unique_ptr<EnhancedSelectorNode> deserialize(std::istream& is);
    };

} // namespace CHTLJS
