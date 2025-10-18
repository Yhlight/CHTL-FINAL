#pragma once

#include "CHTLJS/include/nodes/AstNode.h"
#include <string>

namespace CHTLJS
{
    struct RawJSNode : public AstNode
    {
        std::string content;

        NodeType GetType() const override { return NodeType::RawJS; }
        std::string ToString() const override;
    };

} // namespace CHTLJS
