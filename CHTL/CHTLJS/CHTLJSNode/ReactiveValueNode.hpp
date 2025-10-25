#ifndef REACTIVE_VALUE_NODE_HPP
#define REACTIVE_VALUE_NODE_HPP

#include "CHTLJSNode.hpp"
#include <string>

class ReactiveValueNode : public CHTLJSNode {
public:
    ReactiveValueNode(const std::string& value);
    std::string value;
};

#endif // REACTIVE_VALUE_NODE_HPP
