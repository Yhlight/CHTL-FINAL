#pragma once

#include "AstNode.h"
#include <vector>
#include <memory>
#include <string>

namespace CHTL
{
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

    // 字符串字面量节点
    struct StringLiteral : public Expression
    {
        std::string value;

        NodeType GetType() const override { return NodeType::StringLiteral; }
        std::string ToString() const override { return "\"" + value + "\""; }
    };

    // 条件表达式节点, e.g., condition ? consequence : alternative
    struct ConditionalExpression : public Expression
    {
        std::unique_ptr<Expression> condition;
        std::unique_ptr<Expression> consequence;
        std::unique_ptr<Expression> alternative;

        NodeType GetType() const override { return NodeType::ConditionalExpression; }
        std::string ToString() const override;
    };

    // 变量访问节点, e.g., TemplateName(VariableName)
    struct VariableAccessNode : public Expression
    {
        std::string template_name;
        std::string variable_name;

        NodeType GetType() const override { return NodeType::VariableAccess; }
        std::string ToString() const override;
    };

    // 表达式列表节点
    struct ExpressionListNode : public Expression
    {
        std::vector<std::unique_ptr<Expression>> expressions;

        NodeType GetType() const override { return NodeType::ExpressionList; }
        std::string ToString() const override;
    };
}