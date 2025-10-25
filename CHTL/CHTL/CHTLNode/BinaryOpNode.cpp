#include "BinaryOpNode.hpp"

BinaryOpNode::BinaryOpNode(std::unique_ptr<ExpressionNode> left, const std::string& op, std::unique_ptr<ExpressionNode> right)
    : left(std::move(left)), op(op), right(std::move(right)) {}
