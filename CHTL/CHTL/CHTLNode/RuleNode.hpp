#ifndef CHTL_RULE_NODE_HPP
#define CHTL_RULE_NODE_HPP

#include "AstNode.hpp"
#include "PropertyNode.hpp"
#include <string>
#include <vector>
#include <memory>

class RuleNode : public AstNode {
public:
    RuleNode(const std::string& selector);

    std::string selector;
    std::vector<std::unique_ptr<PropertyNode>> properties;
};

#endif // CHTL_RULE_NODE_HPP
