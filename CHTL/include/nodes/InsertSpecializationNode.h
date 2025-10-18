#pragma once

#include "AstNode.h"
#include <vector>
#include <memory>
#include <string>

namespace CHTL
{
    // 'insert' 特例化节点
    struct InsertSpecializationNode : public AstNode
    {
        std::string position; // "after", "before", "replace", "at top", "at bottom"
        std::string target_selector; // e.g., "div[0]", can be empty for "at top"/"at bottom"
        std::vector<std::unique_ptr<AstNode>> content;

        NodeType GetType() const override { return NodeType::InsertSpecialization; }
        std::string ToString() const override;
    };
}