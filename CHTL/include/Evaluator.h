#pragma once

#include "AstNode.h"
#include "Value.h"
#include <string>
#include <memory>
#include <unordered_map>

namespace CHTL
{
    using EvalContext = std::unordered_map<std::string, Value>;

    class Evaluator
    {
    public:
        Value Eval(Expression* node, EvalContext& context);

    private:
        Value eval(Expression* node, EvalContext& context);
    };
}
