#include "TextNode.h"

namespace CHTL
{
    std::string TextNode::ToString() const
    {
        return "TextNode(\"" + value + "\")";
    }
}