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
        Style,
        Comment,
        StyleProperty,
        StyleRule,
        // Expressions
        NumberLiteral,
        InfixExpression,
        Identifier,
    };

    // AST节点基类
    struct AstNode
    {
        virtual ~AstNode() = default;
        virtual NodeType GetType() const = 0;
        virtual std::string ToString() const = 0;
    };

    // Expression 基类
    struct Expression : public AstNode {};

    // 数字字面量节点
    struct NumberLiteral : public Expression
    {
        double value;
        std::string unit;

        NodeType GetType() const override { return NodeType::NumberLiteral; }
        std::string ToString() const override { return std::to_string(value) + unit; }
    };

    // 中缀表达式节点
    struct InfixExpression : public Expression
    {
        std::unique_ptr<Expression> left;
        std::string op;
        std::unique_ptr<Expression> right;

        NodeType GetType() const override { return NodeType::InfixExpression; }
        std::string ToString() const override {
            return "(" + left->ToString() + " " + op + " " + right->ToString() + ")";
        }
    };

    // 标识符节点
    struct Identifier : public Expression
    {
        std::string value;

        NodeType GetType() const override { return NodeType::Identifier; }
        std::string ToString() const override { return value; }
    };


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

    // 文本节点，例如 text { "content" }
    struct TextNode : public AstNode
    {
        std::string value;

        NodeType GetType() const override { return NodeType::Text; }
        std::string ToString() const override { return "TextNode(\"" + value + "\")"; }
    };

    // 样式属性结构体, e.g., width: 100 + 50;
    struct StyleProperty : public AstNode
    {
        std::string name;
        std::unique_ptr<Expression> value;

        NodeType GetType() const override { return NodeType::StyleProperty; } // Need to add StyleProperty to NodeType
        std::string ToString() const override;
    };

    // CSS 规则节点, e.g., .box { width: 100px; }
    struct StyleRuleNode : public AstNode
    {
        std::string selector;
        std::vector<std::unique_ptr<StyleProperty>> properties;

        NodeType GetType() const override { return NodeType::StyleRule; } // Need to add StyleRule to NodeType
        std::string ToString() const override;
    };

    // 样式节点, e.g., style { ... }
    struct StyleNode : public AstNode
    {
        std::vector<std::unique_ptr<AstNode>> children;

        NodeType GetType() const override { return NodeType::Style; }
        std::string ToString() const override;
    };

    // 注释节点
    struct CommentNode : public AstNode
    {
        std::string value;

        NodeType GetType() const override { return NodeType::Comment; }
        std::string ToString() const override { return "# " + value; }
    };

    // 程序根节点
    struct ProgramNode : public AstNode
    {
        std::vector<std::unique_ptr<AstNode>> children;

        NodeType GetType() const override { return NodeType::Program; }
        std::string ToString() const override;
    };

} // namespace CHTL
