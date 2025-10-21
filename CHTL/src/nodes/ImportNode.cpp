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
}
