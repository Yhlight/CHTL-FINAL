#ifndef CHTL_BINARY_OP_NODE_HPP
#define CHTL_BINARY_OP_NODE_HPP

#include "ExpressionNode.hpp"
#include <memory>

class BinaryOpNode : public ExpressionNode {
public:
    BinaryOpNode(std::unique_ptr<ExpressionNode> left, const std::string& op, std::unique_ptr<ExpressionNode> right);

    std::unique_ptr<ExpressionNode> left;
    std::string op;
    std::unique_ptr<ExpressionNode> right;
};

#endif // CHTL_BINARY_OP_NODE_HPP
