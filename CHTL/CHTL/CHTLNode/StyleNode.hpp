#ifndef CHTL_STYLE_NODE_HPP
#define CHTL_STYLE_NODE_HPP

#include "AstNode.hpp"
#include "PropertyNode.hpp"
#include <vector>
#include <memory>

class StyleNode : public AstNode {
public:
    std::vector<std::unique_ptr<PropertyNode>> properties;
};

#endif // CHTL_STYLE_NODE_HPP
