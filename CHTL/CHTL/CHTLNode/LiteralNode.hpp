#ifndef CHTL_LITERAL_NODE_HPP
#define CHTL_LITERAL_NODE_HPP

#include "ExpressionNode.hpp"
#include <string>

class LiteralNode : public ExpressionNode {
public:
    LiteralNode(const std::string& value);

    std::string value;
};

#endif // CHTL_LITERAL_NODE_HPP
