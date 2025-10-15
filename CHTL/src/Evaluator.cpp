#include "Evaluator.h"
#include <stdexcept>

namespace CHTL
{
    Value Evaluator::Eval(Expression* node, EvalContext& context)
    {
        return eval(node, context);
    }

    Value Evaluator::eval(Expression* node, EvalContext& context)
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
                Value left_val = eval(infix_node->left.get(), context);
                Value right_val = eval(infix_node->right.get(), context);

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
                if (infix_node->op == ">")
                {
                    return left_val > right_val;
                }
                if (infix_node->op == "<")
                {
                    return left_val < right_val;
                }
                throw std::runtime_error("Unknown infix operator: " + infix_node->op);
            }
            case NodeType::Identifier:
            {
                auto ident_node = static_cast<Identifier*>(node);
                if (context.count(ident_node->value))
                {
                    return context.at(ident_node->value);
                }

                // If not in context, treat it as a string literal (e.g., color: red)
                Value val;
                val.type = ValueType::STRING;
                val.str = ident_node->value;
                return val;
            }
            case NodeType::StringLiteral:
            {
                auto str_node = static_cast<StringLiteral*>(node);
                Value val;
                val.type = ValueType::STRING;
                val.str = str_node->value;
                return val;
            }
            case NodeType::ConditionalExpression:
            {
                auto cond_node = static_cast<ConditionalExpression*>(node);
                Value condition_val = eval(cond_node->condition.get(), context);
                if (condition_val.type != ValueType::BOOL)
                {
                    throw std::runtime_error("Conditional expression condition must evaluate to a boolean.");
                }

                if (condition_val.boolean)
                {
                    return eval(cond_node->consequence.get(), context);
                }
                else
                {
                    return eval(cond_node->alternative.get(), context);
                }
            }
            default:
                throw std::runtime_error("Unknown expression node type in Evaluator.");
        }
    }
}
