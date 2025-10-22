#include "CustomUsageNode.h"

namespace CHTL
{
    std::string CustomUsageNode::ToString() const
    {
        return "CustomUsage(" + type + " " + name + ")";
    }

    std::unique_ptr<AstNode> CustomUsageNode::clone() const
    {
        auto node = std::make_unique<CustomUsageNode>();
        node->type = this->type;
        node->name = this->name;
        for (const auto &child : this->specializations)
        {
            node->specializations.push_back(child->clone());
        }
        return node;
    }

    void CustomUsageNode::serialize(std::ostream& os) const
    {
        int node_type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&node_type), sizeof(node_type));

        size_t type_len = type.length();
        os.write(reinterpret_cast<const char*>(&type_len), sizeof(type_len));
        os.write(type.c_str(), type_len);

        size_t name_len = name.length();
        os.write(reinterpret_cast<const char*>(&name_len), sizeof(name_len));
        os.write(name.c_str(), name_len);

        size_t child_count = specializations.size();
        os.write(reinterpret_cast<const char*>(&child_count), sizeof(child_count));
        for (const auto& child : specializations)
        {
            child->serialize(os);
        }
    }

    std::unique_ptr<CustomUsageNode> CustomUsageNode::deserialize(std::istream& is)
    {
        auto node = std::make_unique<CustomUsageNode>();

        size_t type_len;
        is.read(reinterpret_cast<char*>(&type_len), sizeof(type_len));
        node->type.resize(type_len);
        is.read(&node->type[0], type_len);

        size_t name_len;
        is.read(reinterpret_cast<char*>(&name_len), sizeof(name_len));
        node->name.resize(name_len);
        is.read(&node->name[0], name_len);

        size_t child_count;
        is.read(reinterpret_cast<char*>(&child_count), sizeof(child_count));
        for (size_t i = 0; i < child_count; ++i)
        {
            node->specializations.push_back(AstNode::deserialize(is));
        }
        return node;
    }
}