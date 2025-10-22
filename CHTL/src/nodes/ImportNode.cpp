#include "nodes/ImportNode.h"
#include <sstream>

namespace CHTL
{
    std::string ImportNode::ToString() const
    {
        std::stringstream ss;
        ss << "[Import] ";
        if (!import_scope.empty()) {
            ss << import_scope << " ";
        }
        if (!specific_type.empty()) {
            ss << specific_type << " ";
        }
        if (!imported_name.empty()) {
            ss << imported_name << " ";
        }
        if (!type.empty()) {
            ss << type << " ";
        }
        ss << "from \"" << path << "\"";
        if (!alias.empty()) {
            ss << " as " << alias;
        }
        ss << ";";
        return ss.str();
    }

    std::unique_ptr<AstNode> ImportNode::clone() const
    {
        auto node = std::make_unique<ImportNode>();
        node->type = this->type;
        node->import_scope = this->import_scope;
        node->specific_type = this->specific_type;
        node->imported_name = this->imported_name;
        node->alias = this->alias;
        node->path = this->path;
        return node;
    }

    void ImportNode::serialize(std::ostream& os) const
    {
        int node_type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&node_type), sizeof(node_type));

        auto write_string = [&](const std::string& s) {
            size_t len = s.length();
            os.write(reinterpret_cast<const char*>(&len), sizeof(len));
            os.write(s.c_str(), len);
        };

        write_string(type);
        write_string(import_scope);
        write_string(specific_type);
        write_string(imported_name);
        write_string(alias);
        write_string(path);
    }

    std::unique_ptr<ImportNode> ImportNode::deserialize(std::istream& is)
    {
        auto node = std::make_unique<ImportNode>();
        auto read_string = [&](std::string& s) {
            size_t len;
            is.read(reinterpret_cast<char*>(&len), sizeof(len));
            s.resize(len);
            is.read(&s[0], len);
        };

        read_string(node->type);
        read_string(node->import_scope);
        read_string(node->specific_type);
        read_string(node->imported_name);
        read_string(node->alias);
        read_string(node->path);
        return node;
    }
}
