#include "Evaluator.h"
#include "../CHTLNode/Expression.h"
#include <cmath>

namespace CHTL {

    std::unique_ptr<Object> Evaluator::Eval(const Node* node) {
        if (const auto infixExpr = dynamic_cast<const InfixExpression*>(node)) {
            return EvalInfixExpression(infixExpr);
        }
        if (const auto numLiteral = dynamic_cast<const NumberLiteral*>(node)) {
            return EvalNumberLiteral(numLiteral);
        }
        return nullptr;
    }

    std::unique_ptr<Object> Evaluator::EvalInfixExpression(const InfixExpression* node) {
        auto left = Eval(node->Left.get());
        auto right = Eval(node->Right.get());

        if (left->Type == ObjectType::NUMBER && right->Type == ObjectType::NUMBER) {
            if (left->Unit != right->Unit && !left->Unit.empty() && !right->Unit.empty()) {
                // Error: unit mismatch
                return nullptr;
            }
            double result = 0;
            if (node->Operator == "+") {
                result = left->Value + right->Value;
            } else if (node->Operator == "-") {
                result = left->Value - right->Value;
            } else if (node->Operator == "*") {
                result = left->Value * right->Value;
            } else if (node->Operator == "/") {
                result = left->Value / right->Value;
            } else if (node->Operator == "%") {
                result = std::fmod(left->Value, right->Value);
            } else if (node->Operator == "**") {
                result = std::pow(left->Value, right->Value);
            }
            std::string unit = left->Unit.empty() ? right->Unit : left->Unit;
            return std::make_unique<Object>(Object{ObjectType::NUMBER, result, unit});
        }

        return nullptr;
    }

    std::unique_ptr<Object> Evaluator::EvalNumberLiteral(const NumberLiteral* node) {
        return std::make_unique<Object>(Object{ObjectType::NUMBER, node->Value, node->Unit});
    }

} // namespace CHTL
