#include "ElementNode.h"
#include <sstream>

namespace CHTL
{
    std::string ElementNode::ToString() const
    {
        std::stringstream ss;
        ss << "ElementNode(tag: " << tag_name << ", attributes: [";
        for (size_t i = 0; i < attributes.size(); ++i)
        {
            ss << attributes[i].name << ": \"" << attributes[i].value << "\"";
            if (i < attributes.size() - 1)
            {
                ss << ", ";
            }
        }
        ss << "], children: [";
        for (size_t i = 0; i < children.size(); ++i)
        {
            ss << children[i]->ToString();
            if (i < children.size() - 1)
            {
                ss << ", ";
            }
        }
        ss << "])";
        return ss.str();
    }

    std::unique_ptr<AstNode> ElementNode::clone() const
    {
        auto node = std::make_unique<ElementNode>();
        node->tag_name = this->tag_name;
        node->attributes = this->attributes;
        for (const auto& child : children)
        {
            node->children.push_back(child->clone());
        }
        return node;
    }

    void ElementNode::serialize(std::ostream& os) const
    {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));

        size_t tag_len = tag_name.length();
        os.write(reinterpret_cast<const char*>(&tag_len), sizeof(tag_len));
        os.write(tag_name.c_str(), tag_len);

        size_t attr_count = attributes.size();
        os.write(reinterpret_cast<const char*>(&attr_count), sizeof(attr_count));
        for(const auto& attr : attributes)
        {
            size_t name_len = attr.name.length();
            os.write(reinterpret_cast<const char*>(&name_len), sizeof(name_len));
            os.write(attr.name.c_str(), name_len);

            size_t val_len = attr.value.length();
            os.write(reinterpret_cast<const char*>(&val_len), sizeof(val_len));
            os.write(attr.value.c_str(), val_len);
        }

        size_t child_count = children.size();
        os.write(reinterpret_cast<const char*>(&child_count), sizeof(child_count));
        for (const auto& child : children)
        {
            child->serialize(os);
        }
    }

    std::unique_ptr<ElementNode> ElementNode::deserialize(std::istream& is)
    {
        auto node = std::make_unique<ElementNode>();

        size_t tag_len;
        is.read(reinterpret_cast<char*>(&tag_len), sizeof(tag_len));
        node->tag_name.resize(tag_len);
        is.read(&node->tag_name[0], tag_len);

        size_t attr_count;
        is.read(reinterpret_cast<char*>(&attr_count), sizeof(attr_count));
        node->attributes.resize(attr_count);
        for(size_t i = 0; i < attr_count; ++i)
        {
            size_t name_len;
            is.read(reinterpret_cast<char*>(&name_len), sizeof(name_len));
            node->attributes[i].name.resize(name_len);
            is.read(&node->attributes[i].name[0], name_len);

            size_t val_len;
            is.read(reinterpret_cast<char*>(&val_len), sizeof(val_len));
            node->attributes[i].value.resize(val_len);
            is.read(&node->attributes[i].value[0], val_len);
        }

        size_t child_count;
        is.read(reinterpret_cast<char*>(&child_count), sizeof(child_count));
        for (size_t i = 0; i < child_count; ++i)
        {
            node->children.push_back(AstNode::deserialize(is));
        }

        return node;
    }
}