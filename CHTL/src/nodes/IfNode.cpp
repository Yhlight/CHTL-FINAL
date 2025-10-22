#include "nodes/IfNode.h"
#include "nodes/ExpressionNode.h"
#include <sstream>

namespace CHTL
{
    std::string IfNode::ToString() const
    {
        std::stringstream ss;
        ss << "IfNode(condition: " << condition->ToString() << ", consequence: [";
        for (size_t i = 0; i < consequence.size(); ++i)
        {
            ss << consequence[i]->ToString();
            if (i < consequence.size() - 1)
            {
                ss << ", ";
            }
        }
        ss << "])";
        if (alternative)
        {
            ss << " else " << alternative->ToString();
        }
        return ss.str();
    }

    std::unique_ptr<AstNode> IfNode::clone() const
    {
        auto node = std::make_unique<IfNode>();
        if (this->condition)
        {
            node->condition.reset(static_cast<Expression *>(this->condition->clone().release()));
        }
        for (const auto &child : this->consequence)
        {
            node->consequence.push_back(child->clone());
        }
        if (this->alternative)
        {
            node->alternative = this->alternative->clone();
        }
        return node;
    }

    void IfNode::serialize(std::ostream& os) const
    {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));

        condition->serialize(os);
        size_t child_count = consequence.size();
        os.write(reinterpret_cast<const char*>(&child_count), sizeof(child_count));
        for (const auto& child : consequence)
        {
            child->serialize(os);
        }
        bool has_alternative = alternative != nullptr;
        os.write(reinterpret_cast<const char*>(&has_alternative), sizeof(has_alternative));
        if (has_alternative)
        {
            alternative->serialize(os);
        }
    }

    std::unique_ptr<IfNode> IfNode::deserialize(std::istream& is)
    {
        auto node = std::make_unique<IfNode>();
        node->condition = std::unique_ptr<Expression>(static_cast<Expression*>(AstNode::deserialize(is).release()));
        size_t child_count;
        is.read(reinterpret_cast<char*>(&child_count), sizeof(child_count));
        for (size_t i = 0; i < child_count; ++i)
        {
            node->consequence.push_back(AstNode::deserialize(is));
        }
        bool has_alternative;
        is.read(reinterpret_cast<char*>(&has_alternative), sizeof(has_alternative));
        if (has_alternative)
        {
            node->alternative = AstNode::deserialize(is);
        }
        return node;
    }
}