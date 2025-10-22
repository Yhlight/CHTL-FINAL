#pragma once

#include "AstNodeBase.h"
#include <vector>
#include <memory>
#include <string>

namespace CHTL
{
    // Expression 基类
    struct Expression : public AstNode {
         virtual std::unique_ptr<AstNode> clone() const override = 0;
    };

    // 数字字面量节点
    struct NumberLiteral : public Expression
    {
        double value;
        std::string unit;

        NodeType GetType() const override { return NodeType::NumberLiteral; }
        std::string ToString() const override { return std::to_string(value) + unit; }
        std::unique_ptr<AstNode> clone() const override;
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
        std::unique_ptr<AstNode> clone() const override;
    };

    // 标识符节点
    struct Identifier : public Expression
    {
        std::string value;

        NodeType GetType() const override { return NodeType::Identifier; }
        std::string ToString() const override { return value; }
        std::unique_ptr<AstNode> clone() const override;
    };

    // 字符串字面量节点
    struct StringLiteral : public Expression
    {
        std::string value;

        NodeType GetType() const override { return NodeType::StringLiteral; }
        std::string ToString() const override { return "\"" + value + "\""; }
        std::unique_ptr<AstNode> clone() const override;
    };

    // 条件表达式节点, e.g., condition ? consequence : alternative
    struct ConditionalExpression : public Expression
    {
        std::unique_ptr<Expression> condition;
        std::unique_ptr<Expression> consequence;
        std::unique_ptr<Expression> alternative;

        NodeType GetType() const override { return NodeType::ConditionalExpression; }
        std::string ToString() const override;
        std::unique_ptr<AstNode> clone() const override;
    };

    // 变量访问节点, e.g., TemplateName(VariableName)
    struct VariableAccessNode : public Expression
    {
        std::string template_name;
        std::string variable_name;

        NodeType GetType() const override { return NodeType::VariableAccess; }
        std::string ToString() const override;
        std::unique_ptr<AstNode> clone() const override;
    };

    // 表达式列表节点
    struct ExpressionListNode : public Expression
    {
        std::vector<std::unique_ptr<Expression>> expressions;

        NodeType GetType() const override { return NodeType::ExpressionList; }
        std::string ToString() const override;
        std::unique_ptr<AstNode> clone() const override;
    };

    // 属性访问表达式节点, e.g., .box.width
    struct AttributeAccessExpression : public Expression
    {
        std::string selector;
        std::string attribute_name;

        NodeType GetType() const override { return NodeType::AttributeAccess; }
        std::string ToString() const override { return selector + "." + attribute_name; }
        std::unique_ptr<AstNode> clone() const override;
    };
}