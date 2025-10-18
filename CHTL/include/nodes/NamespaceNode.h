#pragma once

#include "AstNode.h"
#include <vector>
#include <memory>
#include <string>

namespace CHTL
{
    struct NamespaceNode : public AstNode
    {
        std::string name;
        std::vector<std::unique_ptr<AstNode>> children;

        NodeType GetType() const override { return NodeType::Namespace; }
        std::string ToString() const override;
    };
}