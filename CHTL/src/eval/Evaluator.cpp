#include "eval/Evaluator.h"
#include <stdexcept>
#include <cmath>
#include <vector>

namespace CHTL
{
    extern const std::string GLOBAL_NAMESPACE;

    // Helper function to recursively find nodes
    void findNodes(AstNode* start_node, const std::string& selector, std::vector<ElementNode*>& found_nodes)
    {
        if (!start_node) return;

        if (start_node->GetType() == NodeType::Element)
        {
            auto* element = static_cast<ElementNode*>(start_node);
            bool match = false;
            if (selector[0] == '#')
            {
                for (const auto& attr : element->attributes)
                {
                    if (attr.name == "id" && attr.value == selector.substr(1))
                    {
                        match = true;
                        break;
                    }
                }
            }
            else if (selector[0] == '.')
            {
                 for (const auto& attr : element->attributes)
                {
                    if (attr.name == "class" && attr.value == selector.substr(1))
                    {
                        match = true;
                        break;
                    }
                }
            }
            else
            {
                if (element->tag_name == selector)
                {
                    match = true;
                }
            }
            if (match)
            {
                found_nodes.push_back(element);
            }
        }

        // Recursively search children
        // (This needs to be adapted based on how children are stored in different node types)
        if (auto* element = dynamic_cast<ElementNode*>(start_node)) {
            for (const auto& child : element->children) {
                findNodes(child.get(), selector, found_nodes);
            }
        } else if (auto* program = dynamic_cast<ProgramNode*>(start_node)) {
             for (const auto& child : program->children) {
                findNodes(child.get(), selector, found_nodes);
            }
        }
    }


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
                if (infix_node->op == "%")
                {
                    if (left_val.type != ValueType::NUMBER || right_val.type != ValueType::NUMBER)
                    {
                        throw std::runtime_error("Modulo operator requires numeric operands.");
                    }
                    Value result;
                    result.type = ValueType::NUMBER;
                    result.num = std::fmod(left_val.num, right_val.num);
                    result.unit = left_val.unit; // Keep unit of the left operand
                    return result;
                }
                if (infix_node->op == "**")
                {
                    if (left_val.type != ValueType::NUMBER || right_val.type != ValueType::NUMBER)
                    {
                         throw std::runtime_error("Power operator requires numeric operands.");
                    }
                    Value result;
                    result.type = ValueType::NUMBER;
                    result.num = std::pow(left_val.num, right_val.num);
                    result.unit = left_val.unit; // Keep unit of the left operand
                    return result;
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
            case NodeType::ExpressionList:
            {
                auto list_node = static_cast<ExpressionListNode*>(node);
                for (const auto& expr : list_node->expressions) {
                    Value result = eval(expr.get(), context);
                    if (result.type != ValueType::EMPTY) {
                        return result;
                    }
                }
                // If all expressions in the list were empty, return the last one (which is empty)
                return Value{ValueType::EMPTY};
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
                    if (cond_node->alternative) {
                        return eval(cond_node->alternative.get(), context);
                    } else {
                        // No 'else' branch, return an empty value to signal this.
                        return Value{ValueType::EMPTY};
                    }
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
                        if (tmpl->type == "@Var") {
                             auto it = tmpl->variables.find(var_access_node->variable_name);
                            if (it != tmpl->variables.end()) {
                                return eval(it->second.get(), context);
                            }
                        } else if (tmpl->type == "@Style") {
                            for (const auto& node : tmpl->properties)
                            {
                                if (auto* prop = dynamic_cast<StyleProperty*>(node.get())) {
                                     if (prop->name == var_access_node->variable_name)
                                    {
                                        return eval(prop->value.get(), context);
                                    }
                                }
                            }
                        }
                    }
                }
                throw std::runtime_error("Variable '" + var_access_node->variable_name + "' not found in template '" + var_access_node->template_name + "'.");
            }
            case NodeType::AttributeAccess:
            {
                auto attr_access_node = static_cast<AttributeAccessExpression*>(node);
                std::vector<ElementNode*> found_nodes;
                if (context.program) {
                    findNodes(const_cast<ProgramNode*>(context.program), attr_access_node->selector, found_nodes);
                }

                if (!found_nodes.empty()) {
                    // For now, just use the first match
                    ElementNode* target_node = found_nodes[0];
                    for (const auto& child : target_node->children) {
                        if (child->GetType() == NodeType::Style) {
                            auto* style_node = static_cast<StyleNode*>(child.get());
                            for (const auto& style_child : style_node->children) {
                                if (style_child->GetType() == NodeType::StyleProperty) {
                                    auto* prop = static_cast<StyleProperty*>(style_child.get());
                                    if (prop->name == attr_access_node->attribute_name) {
                                        return eval(prop->value.get(), context);
                                    }
                                }
                            }
                        }
                    }
                }
                throw std::runtime_error("Attribute '" + attr_access_node->attribute_name + "' not found for selector '" + attr_access_node->selector + "'.");
            }
            case NodeType::DecoupledStringExpression:
            {
                auto decoupled_node = static_cast<DecoupledStringExpression*>(node);
                Value val;
                val.type = ValueType::NUMBER;
                val.num = decoupled_node->number_part->value;
                val.unit = decoupled_node->number_part->unit;
                val.string_template = decoupled_node->string_part;
                return val;
            }
            default:
                throw std::runtime_error("Unknown expression node type in Evaluator.");
        }
    }
}
