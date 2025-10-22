#pragma once

#include "AstNode.h"
#include <vector>
#include <memory>
#include <string>

namespace CHTL
{
    struct ExceptNode; // Forward declaration

    struct NamespaceNode : public AstNode
    {
        std::string name;
        std::vector<std::unique_ptr<AstNode>> children;
        std::vector<std::unique_ptr<ExceptNode>> constraints;

        NodeType GetType() const override { return NodeType::Namespace; }
        std::string ToString() const override;
        std::unique_ptr<AstNode> clone() const override;
    };
}