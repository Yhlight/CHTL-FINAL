#include "InsertSpecializationNode.h"
#include <sstream>

namespace CHTL
{
    std::string InsertSpecializationNode::ToString() const
    {
        std::stringstream ss;
        ss << "InsertSpecializationNode(position: " << position;
        if (!target_selector.empty())
        {
            ss << ", target: " << target_selector;
        }
        ss << ", content: [";
        for (size_t i = 0; i < content.size(); ++i)
        {
            ss << content[i]->ToString();
            if (i < content.size() - 1)
            {
                ss << ", ";
            }
        }
        ss << "])";
        return ss.str();
    }

    std::unique_ptr<AstNode> InsertSpecializationNode::clone() const
    {
        auto node = std::make_unique<InsertSpecializationNode>();
        node->position = this->position;
        node->target_selector = this->target_selector;
        for (const auto &child : this->content)
        {
            node->content.push_back(child->clone());
        }
        return node;
    }

    void InsertSpecializationNode::serialize(std::ostream& os) const
    {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));

        auto write_string = [&](const std::string& s) {
            size_t len = s.length();
            os.write(reinterpret_cast<const char*>(&len), sizeof(len));
            os.write(s.c_str(), len);
        };

        write_string(position);
        write_string(target_selector);

        size_t child_count = content.size();
        os.write(reinterpret_cast<const char*>(&child_count), sizeof(child_count));
        for (const auto& child : content)
        {
            child->serialize(os);
        }
    }

    std::unique_ptr<InsertSpecializationNode> InsertSpecializationNode::deserialize(std::istream& is)
    {
        auto node = std::make_unique<InsertSpecializationNode>();
        auto read_string = [&](std::string& s) {
            size_t len;
            is.read(reinterpret_cast<char*>(&len), sizeof(len));
            s.resize(len);
            is.read(&s[0], len);
        };

        read_string(node->position);
        read_string(node->target_selector);

        size_t child_count;
        is.read(reinterpret_cast<char*>(&child_count), sizeof(child_count));
        for (size_t i = 0; i < child_count; ++i)
        {
            node->content.push_back(AstNode::deserialize(is));
        }
        return node;
    }
}