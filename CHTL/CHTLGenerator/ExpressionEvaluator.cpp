#include "ExpressionEvaluator.h"
#include <cmath>

EvaluatedValue ExpressionEvaluator::evaluate(ExprNode& expr) {
    expr.accept(*this);
    return result;
}

void ExpressionEvaluator::visit(NumericLiteralExprNode& node) {
    result = {node.value, node.unit, "", true};
}

void ExpressionEvaluator::visit(IdentifierExprNode& node) {
    result = {0.0, "", node.name, false};
}

void ExpressionEvaluator::visit(BinaryOpExprNode& node) {
    // Evaluate left and right children
    node.left->accept(*this);
    EvaluatedValue leftResult = result;

    node.right->accept(*this);
    EvaluatedValue rightResult = result;

    if (!leftResult.isNumeric || !rightResult.isNumeric) {
        throw std::runtime_error("Cannot perform arithmetic on non-numeric values.");
    }

    if (leftResult.unit != rightResult.unit && !leftResult.unit.empty() && !rightResult.unit.empty()) {
        throw std::runtime_error("Incompatible units for arithmetic operation: " + leftResult.unit + " and " + rightResult.unit);
    }

    double finalValue = 0.0;
    switch(node.op) {
        case TokenType::PLUS:
            finalValue = leftResult.value + rightResult.value;
            break;
        case TokenType::MINUS:
            finalValue = leftResult.value - rightResult.value;
            break;
        case TokenType::STAR:
            finalValue = leftResult.value * rightResult.value;
            break;
        case TokenType::SLASH:
            if (rightResult.value == 0) throw std::runtime_error("Division by zero.");
            finalValue = leftResult.value / rightResult.value;
            break;
        case TokenType::PERCENT:
            if (rightResult.value == 0) throw std::runtime_error("Division by zero.");
            finalValue = fmod(leftResult.value, rightResult.value);
            break;
        case TokenType::DOUBLE_STAR:
            finalValue = pow(leftResult.value, rightResult.value);
            break;
        default:
            throw std::runtime_error("Unsupported binary operator.");
    }

    result = {finalValue, !leftResult.unit.empty() ? leftResult.unit : rightResult.unit, "", true};
}