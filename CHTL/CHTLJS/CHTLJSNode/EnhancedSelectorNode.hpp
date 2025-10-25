#ifndef ENHANCED_SELECTOR_NODE_HPP
#define ENHANCED_SELECTOR_NODE_HPP

#include "CHTLJSNode.hpp"
#include <string>

class EnhancedSelectorNode : public CHTLJSNode {
public:
    EnhancedSelectorNode(const std::string& selector);
    std::string selector;
};

#endif // ENHANCED_SELECTOR_NODE_HPP
