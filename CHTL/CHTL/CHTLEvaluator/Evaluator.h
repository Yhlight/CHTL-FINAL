#pragma once

#include "Object.h"
#include "../CHTLNode/Node.h"
#include "../CHTLNode/Expression.h"
#include "../CHTLNode/Statement.h"
#include <memory>
#include <map>

namespace CHTL {

    using ElementMap = std::map<std::string, const ElementStatement*>;

    class Evaluator {
    public:
        std::unique_ptr<Object> Eval(const Node* node, std::map<std::string, std::unique_ptr<Object>>& context, const ElementMap& docMap);

    private:
        std::unique_ptr<Object> EvalInfixExpression(const InfixExpression* node, std::map<std::string, std::unique_ptr<Object>>& context, const ElementMap& docMap);
        std::unique_ptr<Object> EvalConditionalExpression(const ConditionalExpression* node, std::map<std::string, std::unique_ptr<Object>>& context, const ElementMap& docMap);
        std::unique_ptr<Object> EvalPropertyAccessExpression(const PropertyAccessExpression* node, std::map<std::string, std::unique_ptr<Object>>& context, const ElementMap& docMap);
        std::unique_ptr<Object> EvalIdentifier(const Identifier* node, std::map<std::string, std::unique_ptr<Object>>& context);
        std::unique_ptr<Object> EvalNumberLiteral(const NumberLiteral* node);
        std::unique_ptr<Object> EvalStringLiteral(const StringLiteral* node);
    };

} // namespace CHTL
