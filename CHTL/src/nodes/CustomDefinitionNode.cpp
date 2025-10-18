#include "nodes/CustomDefinitionNode.h"

namespace CHTL
{
    std::string CustomDefinitionNode::ToString() const
    {
        return "CustomDefinition(" + type + " " + name + ")";
    }
}