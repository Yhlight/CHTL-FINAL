#pragma once

#include "AstNode.h"
#include "Value.h"
#include <memory>

namespace CHTL
{
    class Evaluator
    {
    public:
        Value Eval(Expression* node);

    private:
        Value eval(Expression* node);
    };
}
