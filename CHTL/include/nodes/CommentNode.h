#pragma once

#include "AstNode.h"
#include <string>

namespace CHTL
{
    struct CommentNode : public AstNode
    {
        std::string value;

        NodeType GetType() const override { return NodeType::Comment; }
        std::string ToString() const override { return "# " + value; }
        std::unique_ptr<AstNode> clone() const override;

        void serialize(std::ostream& os) const override;
        static std::unique_ptr<CommentNode> deserialize(std::istream& is);
    };
}