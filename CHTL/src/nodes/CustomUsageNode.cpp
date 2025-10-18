#include "nodes/CustomUsageNode.h"

namespace CHTL
{
    std::string CustomUsageNode::ToString() const
    {
        return "CustomUsage(" + type + " " + name + ")";
    }
}