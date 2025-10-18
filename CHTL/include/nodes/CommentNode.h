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
    };
}