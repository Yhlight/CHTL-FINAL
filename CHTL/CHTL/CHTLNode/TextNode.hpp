#ifndef CHTL_TEXT_NODE_HPP
#define CHTL_TEXT_NODE_HPP

#include "AstNode.hpp"
#include <string>

class TextNode : public AstNode {
public:
    TextNode(const std::string& text);

    std::string text;
};

#endif // CHTL_TEXT_NODE_HPP
