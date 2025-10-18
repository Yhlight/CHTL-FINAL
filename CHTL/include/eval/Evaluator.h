#pragma once

#include "AstNode.h"
#include "Value.h"
#include <string>
#include <memory>
#include <unordered_map>

namespace CHTL
{
    struct ProgramNode; // Forward declaration

    struct EvalContext
    {
        std::unordered_map<std::string, Value> values;
        const ProgramNode* program = nullptr;
        std::string current_namespace;
    };

    class Evaluator
    {
    public:
        Value Eval(Expression* node, EvalContext& context);

    private:
        Value eval(Expression* node, EvalContext& context);
    };
}
