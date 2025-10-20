#include "CHTLJS/include/nodes/RawJSNode.h"

namespace CHTLJS
{
    std::string RawJSNode::ToString() const
    {
        return "RawJSNode(content: \"" + content + "\")";
    }

    std::unique_ptr<AstNode> RawJSNode::clone() const
    {
        auto node = std::make_unique<RawJSNode>();
        node->content = this->content;
        return node;
    }

} // namespace CHTLJS
