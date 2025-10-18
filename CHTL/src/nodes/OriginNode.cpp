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
}