#include "MethodCallNode.hpp"

MethodCallNode::MethodCallNode(std::unique_ptr<CHTLJSNode> object, const std::string& method_name)
    : object(std::move(object)), method_name(method_name) {}
