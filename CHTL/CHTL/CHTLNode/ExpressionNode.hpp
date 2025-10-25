#ifndef CHTL_EXPRESSION_NODE_HPP
#define CHTL_EXPRESSION_NODE_HPP

#include "AstNode.hpp"

class ExpressionNode : public AstNode {
public:
    virtual ~ExpressionNode() = default;
};

#endif // CHTL_EXPRESSION_NODE_HPP
