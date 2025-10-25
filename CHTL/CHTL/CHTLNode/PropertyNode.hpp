#ifndef CHTL_PROPERTY_NODE_HPP
#define CHTL_PROPERTY_NODE_HPP

#include "AstNode.hpp"
#include "ExpressionNode.hpp"
#include <string>
#include <memory>

class PropertyNode : public AstNode {
public:
    PropertyNode(const std::string& key, std::unique_ptr<ExpressionNode> value);

    std::string key;
    std::unique_ptr<ExpressionNode> value;
};

#endif // CHTL_PROPERTY_NODE_HPP
