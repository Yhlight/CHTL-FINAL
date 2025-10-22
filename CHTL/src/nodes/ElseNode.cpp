#include "nodes/ElseNode.h"
#include <sstream>

namespace CHTL
{
    std::string ElseNode::ToString() const
    {
        std::stringstream ss;
        ss << "ElseNode(consequence: [";
        for (size_t i = 0; i < consequence.size(); ++i)
        {
            ss << consequence[i]->ToString();
            if (i < consequence.size() - 1)
            {
                ss << ", ";
            }
        }
        ss << "])";
        return ss.str();
    }

    std::unique_ptr<AstNode> ElseNode::clone() const
    {
        auto node = std::make_unique<ElseNode>();
        for (const auto &child : this->consequence)
        {
            node->consequence.push_back(child->clone());
        }
        return node;
    }

    void ElseNode::serialize(std::ostream& os) const
    {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));

        size_t child_count = consequence.size();
        os.write(reinterpret_cast<const char*>(&child_count), sizeof(child_count));
        for (const auto& child : consequence)
        {
            child->serialize(os);
        }
    }

    std::unique_ptr<ElseNode> ElseNode::deserialize(std::istream& is)
    {
        auto node = std::make_unique<ElseNode>();
        size_t child_count;
        is.read(reinterpret_cast<char*>(&child_count), sizeof(child_count));
        for (size_t i = 0; i < child_count; ++i)
        {
            node->consequence.push_back(AstNode::deserialize(is));
        }
        return node;
    }
}