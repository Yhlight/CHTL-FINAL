#include "Evaluator.h"
#include <stdexcept>

namespace CHTL
{
    double Evaluator::Eval(Expression* node)
    {
        return eval(node);
    }

    double Evaluator::eval(Expression* node)
    {
        if (!node)
        {
            throw std::runtime_error("Evaluator received a null expression node.");
        }

        switch (node->GetType())
        {
            case NodeType::NumberLiteral:
            {
                return static_cast<NumberLiteral*>(node)->value;
            }
            case NodeType::InfixExpression:
            {
                auto infix_node = static_cast<InfixExpression*>(node);
                double left_val = eval(infix_node->left.get());
                double right_val = eval(infix_node->right.get());

                if (infix_node->op == "+")
                {
                    return left_val + right_val;
                }
                else if (infix_node->op == "-")
                {
                    return left_val - right_val;
                }
                else if (infix_node->op == "*")
                {
                    return left_val * right_val;
                }
                else if (infix_node->op == "/")
                {
                    // Handle division by zero if necessary
                    return left_val / right_val;
                }
                else
                {
                    throw std::runtime_error("Unknown infix operator: " + infix_node->op);
                }
            }
            default:
                throw std::runtime_error("Unknown expression node type in Evaluator.");
        }
    }
}
