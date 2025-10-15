#include "Evaluator.h"
#include <stdexcept>

namespace CHTL
{
    Value Evaluator::Eval(Expression* node)
    {
        return eval(node);
    }

    Value Evaluator::eval(Expression* node)
    {
        if (!node)
        {
            throw std::runtime_error("Evaluator received a null expression node.");
        }

        switch (node->GetType())
        {
            case NodeType::NumberLiteral:
            {
                auto num_node = static_cast<NumberLiteral*>(node);
                Value val;
                val.type = ValueType::NUMBER;
                val.num = num_node->value;
                val.unit = num_node->unit;
                return val;
            }
            case NodeType::InfixExpression:
            {
                auto infix_node = static_cast<InfixExpression*>(node);
                Value left_val = eval(infix_node->left.get());
                Value right_val = eval(infix_node->right.get());

                if (infix_node->op == "+")
                {
                    return left_val + right_val;
                }
                if (infix_node->op == "-")
                {
                    return left_val - right_val;
                }
                if (infix_node->op == "*")
                {
                    return left_val * right_val;
                }
                if (infix_node->op == "/")
                {
                    return left_val / right_val;
                }
                throw std::runtime_error("Unknown infix operator: " + infix_node->op);
            }
            case NodeType::Identifier:
            {
                auto ident_node = static_cast<Identifier*>(node);
                Value val;
                val.type = ValueType::STRING;
                val.str = ident_node->value;
                return val;
            }
            default:
                throw std::runtime_error("Unknown expression node type in Evaluator.");
        }
    }
}
