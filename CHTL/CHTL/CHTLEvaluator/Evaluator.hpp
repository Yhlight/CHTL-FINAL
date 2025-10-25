#ifndef CHTL_EVALUATOR_HPP
#define CHTL_EVALUATOR_HPP

#include "CHTLNode/ExpressionNode.hpp"
#include <string>

class Evaluator {
public:
    std::string evaluate(const ExpressionNode& node);
};

#endif // CHTL_EVALUATOR_HPP
