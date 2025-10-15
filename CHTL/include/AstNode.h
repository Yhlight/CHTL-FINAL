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
        // Expressions
        NumberLiteral,
        InfixExpression,
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

    // Statement 基类
    struct StatementNode : public AstNode {};

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


    // 属性结构体
    struct Attribute
    {
        std::string name;
        std::string value;
    };

    // 元素节点，例如 div { ... }
    struct ElementNode : public StatementNode
    {
        std::string tagName;
        std::vector<Attribute> attributes;
        std::vector<std::unique_ptr<StatementNode>> children;

        NodeType GetType() const override { return NodeType::Element; }
        std::string ToString() const override;
    };

    // 文本节点，例如 text { "content" }
    struct TextNode : public StatementNode
    {
        std::string value;

        NodeType GetType() const override { return NodeType::Text; }
        std::string ToString() const override { return "TextNode(\"" + value + "\")"; }
    };

    // 样式属性结构体, e.g., width: 100 + 50;
    struct StyleProperty
    {
        std::string name;
        std::unique_ptr<Expression> value;
    };

    // 样式节点, e.g., style { ... }
    struct StyleNode : public StatementNode
    {
        std::vector<StyleProperty> properties;

        NodeType GetType() const override { return NodeType::Style; }
        std::string ToString() const override;
    };

    // 程序根节点
    struct ProgramNode : public AstNode
    {
        std::vector<std::unique_ptr<StatementNode>> statements;

        NodeType GetType() const override { return NodeType::Program; }
        std::string ToString() const override;
    };

} // namespace CHTL
