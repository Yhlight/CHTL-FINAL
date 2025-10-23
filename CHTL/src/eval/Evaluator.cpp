#include "eval/Evaluator.h"
#include <stdexcept>
#include <cmath>
#include <vector>
#include <sstream>
#include <set>
#include <optional>
#include <sstream>
#include <set>
#include <algorithm>

namespace CHTL
{
    extern const std::string GLOBAL_NAMESPACE;

    // Checks if a single element matches a simple selector (e.g., #id, .class, tag)
    bool nodeMatchesSelector(const ElementNode* element, const std::string& selector_part) {
        if (!element || selector_part.empty()) return false;

        char first_char = selector_part[0];
        std::string value = selector_part.substr(1);

        if (first_char == '#') {
            return std::any_of(element->attributes.begin(), element->attributes.end(),
                [&](const Attribute& attr) { return attr.name == "id" && attr.value == value; });
        }
        if (first_char == '.') {
             return std::any_of(element->attributes.begin(), element->attributes.end(),
                [&](const Attribute& attr) { return attr.name == "class" && attr.value == value; });
        }
        return element->tag_name == selector_part;
    }

    // Helper to get all descendant ElementNodes of a given node, including the node itself if it matches.
    void getAllDescendants(const AstNode* node, std::vector<ElementNode*>& descendants) {
        if (!node) return;

        // Add the node itself if it's an element
        if (auto* element = dynamic_cast<const ElementNode*>(node)) {
             for (const auto& child : element->children) {
                if (child->GetType() == NodeType::Element) {
                    descendants.push_back(static_cast<ElementNode*>(child.get()));
                }
                getAllDescendants(child.get(), descendants);
            }
        } else if (auto* program = dynamic_cast<const ProgramNode*>(node)) {
             for (const auto& child : program->children) {
                if (child->GetType() == NodeType::Element) {
                    descendants.push_back(static_cast<ElementNode*>(child.get()));
                }
                getAllDescendants(child.get(), descendants);
            }
        }
    }

    void findNodes(AstNode* start_node, const std::string& selector, std::vector<ElementNode*>& found_nodes) {
        std::stringstream ss(selector);
        std::string part;
        std::vector<std::string> parts;
        while (ss >> part) {
            parts.push_back(part);
        }

        if (parts.empty()) return;

        std::vector<ElementNode*> current_matches;

        // Initial search space
        if (start_node->GetType() == NodeType::Element) {
            current_matches.push_back(static_cast<ElementNode*>(start_node));
        }
        getAllDescendants(start_node, current_matches);

        // Filter initial space by the first selector part
        std::vector<ElementNode*> filtered_matches;
        for(auto* node : current_matches) {
            if(nodeMatchesSelector(node, parts[0])) {
                filtered_matches.push_back(node);
            }
        }
        current_matches = filtered_matches;

        // Process subsequent selector parts
        for (size_t i = 1; i < parts.size(); ++i) {
            if (current_matches.empty()) break;

            std::vector<ElementNode*> next_matches;
            for (ElementNode* matched_node : current_matches) {
                // The search space for the next part is the descendants of the current match
                std::vector<ElementNode*> search_space;
                getAllDescendants(matched_node, search_space);
                for (ElementNode* descendant : search_space) {
                    if (nodeMatchesSelector(descendant, parts[i])) {
                        next_matches.push_back(descendant);
                    }
                }
            }
            current_matches = next_matches;
        }

        // Deduplicate and assign results, preserving order
        std::set<ElementNode*> seen_nodes;
        found_nodes.clear();
        for (ElementNode* node : current_matches) {
            if (seen_nodes.find(node) == seen_nodes.end()) {
                seen_nodes.insert(node);
                found_nodes.push_back(node);
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

                // Separate selector from index, e.g., ".button[1]" -> ".button" and 1
                std::string base_selector = attr_access_node->selector;
                int index = 0; // Default to the first element if no index is specified
                size_t bracket_pos = base_selector.find('[');
                if (bracket_pos != std::string::npos) {
                    size_t end_bracket_pos = base_selector.find(']');
                    if (end_bracket_pos != std::string::npos && end_bracket_pos > bracket_pos + 1) {
                        try {
                            index = std::stoi(base_selector.substr(bracket_pos + 1, end_bracket_pos - bracket_pos - 1));
                        } catch (...) {
                             throw std::runtime_error("Invalid index format in selector: " + attr_access_node->selector);
                        }
                        base_selector = base_selector.substr(0, bracket_pos);
                    }
                }

                std::vector<ElementNode*> found_nodes;
                if (context.program) {
                    findNodes(const_cast<ProgramNode*>(context.program), base_selector, found_nodes);
                }

                if (index < found_nodes.size()) {
                    ElementNode* target_node = found_nodes[index];
                    if (target_node) {
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
