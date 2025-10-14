#pragma once

#include "AstNode.h"
#include <memory>

namespace CHTL
{
    class Evaluator
    {
    public:
        double Eval(Expression* node);

    private:
        double eval(Expression* node);
    };
}
