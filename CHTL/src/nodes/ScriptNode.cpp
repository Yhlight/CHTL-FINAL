#include "ScriptNode.h"
#include "CHTLJS/include/nodes/AstNode.h"

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

    void ScriptNode::serialize(std::ostream& os) const
    {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));

        bool has_js_ast = js_ast != nullptr;
        os.write(reinterpret_cast<const char*>(&has_js_ast), sizeof(has_js_ast));
        if (has_js_ast)
        {
            js_ast->serialize(os);
        }
    }

    std::unique_ptr<ScriptNode> ScriptNode::deserialize(std::istream& is)
    {
        auto node = std::make_unique<ScriptNode>();
        bool has_js_ast;
        is.read(reinterpret_cast<char*>(&has_js_ast), sizeof(has_js_ast));
        if (has_js_ast)
        {
            node->js_ast = std::unique_ptr<CHTLJS::ProgramNode>(static_cast<CHTLJS::ProgramNode*>(CHTLJS::AstNode::deserialize(is).release()));
        }
        return node;
    }
}