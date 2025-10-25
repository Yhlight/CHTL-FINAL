#include "PropertyNode.hpp"

PropertyNode::PropertyNode(const std::string& key, std::unique_ptr<ExpressionNode> value) : key(key), value(std::move(value)) {}
