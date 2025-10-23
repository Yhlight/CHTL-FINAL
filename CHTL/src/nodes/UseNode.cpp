#include "nodes/UseNode.h"
#include <sstream>

namespace CHTL
{
    std::string UseNode::ToString() const
    {
        std::stringstream ss;
        ss << "<UseNode path=[";
        for (size_t i = 0; i < path.size(); ++i)
        {
            ss << path[i];
            if (i < path.size() - 1)
            {
                ss << ", ";
            }
        }
        ss << "] />";
        return ss.str();
    }

    std::unique_ptr<AstNode> UseNode::clone() const
    {
        auto node = std::make_unique<UseNode>();
        node->path = path;
        return node;
    }

    void UseNode::serialize(std::ostream& os) const
    {
        // Serialization logic for UseNode, if needed.
        // For now, this is a placeholder.
    }
}
