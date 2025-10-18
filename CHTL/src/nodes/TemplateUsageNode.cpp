#include "nodes/TemplateUsageNode.h"

namespace CHTL
{
    std::string TemplateUsageNode::ToString() const
    {
        return "TemplateUsage(" + type + " " + name + ")";
    }
}