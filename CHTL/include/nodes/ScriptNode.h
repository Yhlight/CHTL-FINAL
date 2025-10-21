#pragma once

#include "AstNode.h"
#include "CHTLJS/include/nodes/AstNode.h"
#include "CHTLJS/include/nodes/ProgramNode.h"
#include <string>
#include <memory>

namespace CHTL
{
    struct ScriptNode : public AstNode
    {
        std::unique_ptr<CHTLJS::ProgramNode> js_ast;

        NodeType GetType() const override { return NodeType::Script; }
        std::string ToString() const override;
        std::unique_ptr<AstNode> clone() const override;
    };
}