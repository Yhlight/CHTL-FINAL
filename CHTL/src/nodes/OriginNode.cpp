#include "OriginNode.h"

namespace CHTL
{
    std::string OriginNode::ToString() const
    {
        std::string result = "[Origin] " + type;
        if (!name.empty())
        {
            result += " " + name;
        }
        result += " { " + content + " }";
        return result;
    }

    std::unique_ptr<AstNode> OriginNode::clone() const
    {
        auto node = std::make_unique<OriginNode>();
        node->type = this->type;
        node->name = this->name;
        node->content = this->content;
        return node;
    }

    void OriginNode::serialize(std::ostream& os) const
    {
        int node_type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&node_type), sizeof(node_type));

        auto write_string = [&](const std::string& s) {
            size_t len = s.length();
            os.write(reinterpret_cast<const char*>(&len), sizeof(len));
            os.write(s.c_str(), len);
        };

        write_string(type);
        write_string(name);
        write_string(content);
    }

    std::unique_ptr<OriginNode> OriginNode::deserialize(std::istream& is)
    {
        auto node = std::make_unique<OriginNode>();
        auto read_string = [&](std::string& s) {
            size_t len;
            is.read(reinterpret_cast<char*>(&len), sizeof(len));
            s.resize(len);
            is.read(&s[0], len);
        };

        read_string(node->type);
        read_string(node->name);
        read_string(node->content);
        return node;
    }
}