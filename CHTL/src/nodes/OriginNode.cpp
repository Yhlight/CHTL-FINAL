#include "OriginNode.h"

namespace CHTL
{
    std::string OriginNode::ToString() const
    {
        std::string result = "[Origin] " + type;
        if (!name.empty())
        {
            result += " " + name;
        }
        result += " { " + content + " }";
        return result;
    }

    std::unique_ptr<AstNode> OriginNode::clone() const
    {
        auto node = std::make_unique<OriginNode>();
        node->type = this->type;
        node->name = this->name;
        node->content = this->content;
        return node;
    }
}