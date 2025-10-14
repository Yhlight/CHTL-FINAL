#pragma once

#include "Object.h"
#include "../CHTLNode/Node.h"
#include "../CHTLNode/Expression.h"
#include <memory>
#include <map>

namespace CHTL {

    class Evaluator {
    public:
        std::unique_ptr<Object> Eval(const Node* node, std::map<std::string, std::unique_ptr<Object>>& context);

    private:
        std::unique_ptr<Object> EvalInfixExpression(const InfixExpression* node, std::map<std::string, std::unique_ptr<Object>>& context);
        std::unique_ptr<Object> EvalConditionalExpression(const ConditionalExpression* node, std::map<std::string, std::unique_ptr<Object>>& context);
        std::unique_ptr<Object> EvalIdentifier(const Identifier* node, std::map<std::string, std::unique_ptr<Object>>& context);
        std::unique_ptr<Object> EvalNumberLiteral(const NumberLiteral* node);
        std::unique_ptr<Object> EvalStringLiteral(const StringLiteral* node);
    };

} // namespace CHTL
