#include "Evaluator.h"
#include "../CHTLNode/Expression.h"
#include <cmath>

namespace CHTL {

    std::unique_ptr<Object> Evaluator::Eval(const Node* node, std::map<std::string, std::unique_ptr<Object>>& context) {
        if (const auto condExpr = dynamic_cast<const ConditionalExpression*>(node)) {
            return EvalConditionalExpression(condExpr, context);
        }
        if (const auto infixExpr = dynamic_cast<const InfixExpression*>(node)) {
            return EvalInfixExpression(infixExpr, context);
        }
        if (const auto numLiteral = dynamic_cast<const NumberLiteral*>(node)) {
            return EvalNumberLiteral(numLiteral);
        }
        if (const auto strLiteral = dynamic_cast<const StringLiteral*>(node)) {
            return EvalStringLiteral(strLiteral);
        }
        if (const auto ident = dynamic_cast<const Identifier*>(node)) {
            return EvalIdentifier(ident, context);
        }
        return nullptr;
    }

    std::unique_ptr<Object> Evaluator::EvalConditionalExpression(const ConditionalExpression* node, std::map<std::string, std::unique_ptr<Object>>& context) {
        auto condition = Eval(node->Condition.get(), context);
        if (condition && condition->IsTruthy()) {
            return Eval(node->Consequence.get(), context);
        } else {
            return Eval(node->Alternative.get(), context);
        }
    }

    std::unique_ptr<Object> Evaluator::EvalInfixExpression(const InfixExpression* node, std::map<std::string, std::unique_ptr<Object>>& context) {
        if (node->Operator == "&&") {
            auto left = Eval(node->Left.get(), context);
            if (left && !left->IsTruthy()) return std::make_unique<Object>(Object{ObjectType::BOOLEAN, 0, "", false});
            return Eval(node->Right.get(), context);
        }
        if (node->Operator == "||") {
            auto left = Eval(node->Left.get(), context);
            if (left && left->IsTruthy()) return left;
            return Eval(node->Right.get(), context);
        }

        auto left = Eval(node->Left.get(), context);
        auto right = Eval(node->Right.get(), context);

        if (!left || !right) return nullptr;

        if (left->Type == ObjectType::NUMBER && right->Type == ObjectType::NUMBER) {
            if (left->Unit != right->Unit && !left->Unit.empty() && !right->Unit.empty()) {
                // Error: unit mismatch
                return nullptr;
            }
            double result_val = 0;
            bool result_bool = false;
            ObjectType result_type = ObjectType::NUMBER;

            if (node->Operator == "+") {
                result_val = left->Value + right->Value;
            } else if (node->Operator == "-") {
                result_val = left->Value - right->Value;
            } else if (node->Operator == "*") {
                result_val = left->Value * right->Value;
            } else if (node->Operator == "/") {
                result_val = left->Value / right->Value;
            } else if (node->Operator == "%") {
                result_val = std::fmod(left->Value, right->Value);
            } else if (node->Operator == "**") {
                result_val = std::pow(left->Value, right->Value);
            } else if (node->Operator == "<") {
                result_type = ObjectType::BOOLEAN;
                result_bool = left->Value < right->Value;
            } else if (node->Operator == ">") {
                result_type = ObjectType::BOOLEAN;
                result_bool = left->Value > right->Value;
            } else if (node->Operator == "==") {
                result_type = ObjectType::BOOLEAN;
                result_bool = left->Value == right->Value;
            } else if (node->Operator == "!=") {
                result_type = ObjectType::BOOLEAN;
                result_bool = left->Value != right->Value;
            }

            std::string unit = left->Unit.empty() ? right->Unit : left->Unit;
            if (result_type == ObjectType::NUMBER) {
                 return std::make_unique<Object>(Object{ObjectType::NUMBER, result_val, unit, false, ""});
            } else {
                 return std::make_unique<Object>(Object{ObjectType::BOOLEAN, 0, "", result_bool, ""});
            }
        }

        return nullptr;
    }

    std::unique_ptr<Object> Evaluator::EvalIdentifier(const Identifier* node, std::map<std::string, std::unique_ptr<Object>>& context) {
        if (context.count(node->value)) {
            // This is a simplified implementation. We should be returning a copy.
            Object* obj = context[node->value].get();
             return std::make_unique<Object>(Object{obj->Type, obj->Value, obj->Unit, obj->BoolValue, obj->StringValue});
        }
        return nullptr;
    }

    std::unique_ptr<Object> Evaluator::EvalNumberLiteral(const NumberLiteral* node) {
        return std::make_unique<Object>(Object{ObjectType::NUMBER, node->Value, node->Unit, false, ""});
    }

    std::unique_ptr<Object> Evaluator::EvalStringLiteral(const StringLiteral* node) {
        return std::make_unique<Object>(Object{ObjectType::STRING, 0, "", false, node->value});
    }

} // namespace CHTL
