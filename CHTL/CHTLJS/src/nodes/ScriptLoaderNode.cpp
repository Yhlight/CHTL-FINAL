#include "CHTLJS/include/nodes/ScriptLoaderNode.h"

namespace CHTLJS
{
    std::unique_ptr<AstNode> ScriptLoaderNode::clone() const
    {
        auto node = std::make_unique<ScriptLoaderNode>();
        node->paths = paths;
        return node;
    }
}
