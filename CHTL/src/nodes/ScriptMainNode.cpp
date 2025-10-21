#include "nodes/ScriptMainNode.h"

namespace CHTL
{
    std::string ScriptMainNode::ToString() const
    {
        return "ScriptMainNode";
    }

    std::unique_ptr<AstNode> ScriptMainNode::clone() const
    {
        auto node = std::make_unique<ScriptMainNode>();
        if (this->js_ast) {
            node->js_ast = std::unique_ptr<CHTLJS::ProgramNode>(static_cast<CHTLJS::ProgramNode*>(this->js_ast->clone().release()));
        }
        return node;
    }
}
