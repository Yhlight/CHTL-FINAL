#pragma once

#include "Object.h"
#include "../CHTLNode/Node.h"
#include "../CHTLNode/Expression.h"
#include <memory>

namespace CHTL {

    class Evaluator {
    public:
        std::unique_ptr<Object> Eval(const Node* node);

    private:
        std::unique_ptr<Object> EvalInfixExpression(const InfixExpression* node);
        std::unique_ptr<Object> EvalNumberLiteral(const NumberLiteral* node);
    };

} // namespace CHTL
