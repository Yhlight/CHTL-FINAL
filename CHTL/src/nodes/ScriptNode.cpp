#include "ScriptNode.h"

namespace CHTL
{
    std::string ScriptNode::ToString() const
    {
        return "ScriptNode";
    }

    std::unique_ptr<AstNode> ScriptNode::clone() const
    {
        auto node = std::make_unique<ScriptNode>();
        // The js_ast is a unique_ptr and should be cloned if it exists.
        if (this->js_ast) {
            node->js_ast = std::unique_ptr<CHTLJS::ProgramNode>(static_cast<CHTLJS::ProgramNode*>(this->js_ast->clone().release()));
        }
        return node;
    }
}