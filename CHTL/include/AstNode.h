#pragma once

#include <vector>
#include <memory>
#include <string>

namespace CHTL
{
    // AST节点类型
    enum class NodeType
    {
        Program,
        Element,
        Text,
    };

    // AST节点基类
    struct AstNode
    {
        virtual ~AstNode() = default;
        virtual NodeType GetType() const = 0;
        virtual std::string ToString() const = 0;
    };

    // 元素节点，例如 div { ... }
    struct ElementNode : public AstNode
    {
        std::string tag_name;
        std::vector<std::unique_ptr<AstNode>> children;

        NodeType GetType() const override { return NodeType::Element; }
        std::string ToString() const override;
    };

    // 文本节点，例如 text { "content" }
    struct TextNode : public AstNode
    {
        std::string value;

        NodeType GetType() const override { return NodeType::Text; }
        std::string ToString() const override { return "TextNode(\"" + value + "\")"; }
    };

    // 程序根节点
    struct ProgramNode : public AstNode
    {
        std::vector<std::unique_ptr<AstNode>> children;

        NodeType GetType() const override { return NodeType::Program; }
        std::string ToString() const override;
    };

} // namespace CHTL
