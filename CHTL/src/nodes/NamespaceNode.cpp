#include "nodes/NamespaceNode.h"

namespace CHTL
{
    std::string NamespaceNode::ToString() const
    {
        return "[Namespace] " + name + " { ... }";
    }
}