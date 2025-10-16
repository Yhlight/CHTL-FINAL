#include "Evaluator.h"
#include <stdexcept>

namespace CHTL
{
    extern const std::string GLOBAL_NAMESPACE;

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
                if (context.values.count(ident_node->value))
                {
                    return context.values.at(ident_node->value);
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
            case NodeType::VariableAccess:
            {
                auto var_access_node = static_cast<VariableAccessNode*>(node);
                if (context.program)
                {
                    const TemplateDefinitionNode* tmpl = nullptr;
                    // 1. Look in current namespace
                    if (context.program->templates.count(context.current_namespace) && context.program->templates.at(context.current_namespace).count(var_access_node->template_name))
                    {
                        tmpl = context.program->templates.at(context.current_namespace).at(var_access_node->template_name);
                    }
                    // 2. Look in global namespace
                    else if (context.current_namespace != GLOBAL_NAMESPACE && context.program->templates.count(GLOBAL_NAMESPACE) && context.program->templates.at(GLOBAL_NAMESPACE).count(var_access_node->template_name))
                    {
                        tmpl = context.program->templates.at(GLOBAL_NAMESPACE).at(var_access_node->template_name);
                    }

                    if (tmpl) {
                        for (const auto& prop : tmpl->properties)
                        {
                            if (prop->name == var_access_node->variable_name)
                            {
                                return eval(prop->value.get(), context);
                            }
                        }
                    }
                }
                throw std::runtime_error("Variable '" + var_access_node->variable_name + "' not found in template '" + var_access_node->template_name + "'.");
            }
            default:
                throw std::runtime_error("Unknown expression node type in Evaluator.");
        }
    }
}
