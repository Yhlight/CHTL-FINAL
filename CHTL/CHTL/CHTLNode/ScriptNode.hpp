#ifndef CHTL_SCRIPT_NODE_HPP
#define CHTL_SCRIPT_NODE_HPP

#include "AstNode.hpp"
#include <string>

class ScriptNode : public AstNode {
public:
    ScriptNode(const std::string& content);

    std::string content;
};

#endif // CHTL_SCRIPT_NODE_HPP
