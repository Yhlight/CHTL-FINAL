#pragma once

#include "AstNode.h"
#include <vector>
#include <memory>

namespace CHTL
{
    struct ElseNode : public AstNode
    {
        std::vector<std::unique_ptr<AstNode>> consequence;

        NodeType GetType() const override { return NodeType::Else; }
        std::string ToString() const override;
        std::unique_ptr<AstNode> clone() const override;

        void serialize(std::ostream& os) const override;
        static std::unique_ptr<ElseNode> deserialize(std::istream& is);
    };
}