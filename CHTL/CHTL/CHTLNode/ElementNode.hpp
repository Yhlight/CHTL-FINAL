#ifndef CHTL_ELEMENT_NODE_HPP
#define CHTL_ELEMENT_NODE_HPP

#include "AstNode.hpp"
#include <string>
#include <vector>
#include <map>

class ElementNode : public AstNode {
public:
    ElementNode(const std::string& tag_name);

    std::string tag_name;
    std::map<std::string, std::string> attributes;
    std::vector<std::unique_ptr<AstNode>> children;
};

#endif // CHTL_ELEMENT_NODE_HPP
