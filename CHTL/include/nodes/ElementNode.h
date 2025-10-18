#pragma once

#include "AstNode.h"
#include <vector>
#include <memory>
#include <string>

namespace CHTL
{
    // 属性结构体
    struct Attribute
    {
        std::string name;
        std::string value;
    };

    // 元素节点，例如 div { ... }
    struct ElementNode : public AstNode
    {
        std::string tag_name;
        std::vector<Attribute> attributes;
        std::vector<std::unique_ptr<AstNode>> children;

        NodeType GetType() const override { return NodeType::Element; }
        std::string ToString() const override;
    };
}