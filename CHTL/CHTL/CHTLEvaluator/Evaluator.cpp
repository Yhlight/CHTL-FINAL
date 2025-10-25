#include "Evaluator.hpp"
#include "CHTLNode/LiteralNode.hpp"
#include "CHTLNode/BinaryOpNode.hpp"
#include <iostream>
#include <string>
#include <utility>
#include <sstream>

std::pair<double, std::string> parseValue(const std::string& value) {
    size_t i = 0;
    while (i < value.size() && (std::isdigit(value[i]) || value[i] == '.')) {
        i++;
    }
    double number = std::stod(value.substr(0, i));
    std::string unit = value.substr(i);
    return {number, unit};
}

std::string Evaluator::evaluate(const ExpressionNode& node) {
    if (const LiteralNode* literal = dynamic_cast<const LiteralNode*>(&node)) {
        return literal->value;
    } else if (const BinaryOpNode* bin_op = dynamic_cast<const BinaryOpNode*>(&node)) {
        auto left = parseValue(evaluate(*bin_op->left));
        auto right = parseValue(evaluate(*bin_op->right));

        if (left.second != right.second && !left.second.empty() && !right.second.empty()) {
            // This should be an error
            return "error";
        }

        double result;
        if (bin_op->op == "+") {
            result = left.first + right.first;
        } else if (bin_op->op == "-") {
            result = left.first - right.first;
        } else if (bin_op->op == "*") {
            result = left.first * right.first;
        } else if (bin_op->op == "/") {
            result = left.first / right.first;
        } else {
            // This should be an error
            return "error";
        }
        std::stringstream ss;
        ss << result;
        return ss.str() + (left.second.empty() ? right.second : left.second);
    }
    return "";
}
