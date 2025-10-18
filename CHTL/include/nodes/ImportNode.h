#pragma once

#include "AstNode.h"
#include <string>

namespace CHTL
{
    struct ImportNode : public AstNode
    {
        std::string type; // e.g., "@Chtl"
        std::string path;

        NodeType GetType() const override { return NodeType::Import; }
        std::string ToString() const override { return "[Import] " + type + " from \"" + path + "\";"; }
    };
}