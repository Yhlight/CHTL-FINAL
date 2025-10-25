#ifndef CHTL_PROPERTY_NODE_HPP
#define CHTL_PROPERTY_NODE_HPP

#include "AstNode.hpp"
#include <string>

class PropertyNode : public AstNode {
public:
    PropertyNode(const std::string& key, const std::string& value);

    std::string key;
    std::string value;
};

#endif // CHTL_PROPERTY_NODE_HPP
