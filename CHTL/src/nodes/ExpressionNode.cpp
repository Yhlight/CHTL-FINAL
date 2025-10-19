#include "ExpressionNode.h"
#include <sstream>

namespace CHTL
{
    std::string ConditionalExpression::ToString() const
    {
        std::stringstream ss;
        ss << "(" << condition->ToString() << " ? " << consequence->ToString();
        if (alternative)
        {
            ss << " : " << alternative->ToString();
        }
        ss << ")";
        return ss.str();
    }

    std::string VariableAccessNode::ToString() const
    {
        return template_name + "(" + variable_name + ")";
    }

    std::string ExpressionListNode::ToString() const
    {
        std::stringstream ss;
        ss << "ExpressionList([";
        for (size_t i = 0; i < expressions.size(); ++i)
        {
            ss << expressions[i]->ToString();
            if (i < expressions.size() - 1)
            {
                ss << ", ";
            }
        }
        ss << "])";
        return ss.str();
    }

    std::unique_ptr<AstNode> NumberLiteral::clone() const {
        auto node = std::make_unique<NumberLiteral>();
        node->value = this->value;
        node->unit = this->unit;
        return node;
    }

    std::unique_ptr<AstNode> InfixExpression::clone() const {
        auto node = std::make_unique<InfixExpression>();
        node->left = std::unique_ptr<Expression>(static_cast<Expression*>(this->left->clone().release()));
        node->op = this->op;
        node->right = std::unique_ptr<Expression>(static_cast<Expression*>(this->right->clone().release()));
        return node;
    }

    std::unique_ptr<AstNode> Identifier::clone() const {
        auto node = std::make_unique<Identifier>();
        node->value = this->value;
        return node;
    }

    std::unique_ptr<AstNode> StringLiteral::clone() const {
        auto node = std::make_unique<StringLiteral>();
        node->value = this->value;
        return node;
    }

    std::unique_ptr<AstNode> ConditionalExpression::clone() const {
        auto node = std::make_unique<ConditionalExpression>();
        node->condition = std::unique_ptr<Expression>(static_cast<Expression*>(this->condition->clone().release()));
        node->consequence = std::unique_ptr<Expression>(static_cast<Expression*>(this->consequence->clone().release()));
        if (this->alternative) {
            node->alternative = std::unique_ptr<Expression>(static_cast<Expression*>(this->alternative->clone().release()));
        }
        return node;
    }

    std::unique_ptr<AstNode> VariableAccessNode::clone() const {
        auto node = std::make_unique<VariableAccessNode>();
        node->template_name = this->template_name;
        node->variable_name = this->variable_name;
        return node;
    }

    std::unique_ptr<AstNode> ExpressionListNode::clone() const {
        auto node = std::make_unique<ExpressionListNode>();
        for (const auto& expr : this->expressions) {
            node->expressions.push_back(std::unique_ptr<Expression>(static_cast<Expression*>(expr->clone().release())));
        }
        return node;
    }

    std::unique_ptr<AstNode> AttributeAccessExpression::clone() const {
        auto node = std::make_unique<AttributeAccessExpression>();
        node->selector = this->selector;
        node->attribute_name = this->attribute_name;
        return node;
    }
}