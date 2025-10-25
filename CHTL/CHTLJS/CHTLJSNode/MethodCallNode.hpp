#ifndef METHOD_CALL_NODE_HPP
#define METHOD_CALL_NODE_HPP

#include "CHTLJSNode.hpp"
#include <string>
#include <vector>
#include <memory>

class MethodCallNode : public CHTLJSNode {
public:
    MethodCallNode(std::unique_ptr<CHTLJSNode> object, const std::string& method_name);
    std::unique_ptr<CHTLJSNode> object;
    std::string method_name;
};

#endif // METHOD_CALL_NODE_HPP
