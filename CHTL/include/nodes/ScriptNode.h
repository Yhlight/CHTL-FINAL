#pragma once

#include "AstNode.h"
#include <string>

namespace CHTL
{
    struct ScriptNode : public AstNode
    {
        std::string content;

        NodeType GetType() const override { return NodeType::Script; }
        std::string ToString() const override;
    };
}