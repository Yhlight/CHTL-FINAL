#include "ExceptNode.h"
#include <sstream>

namespace CHTL
{
    std::string ExceptNode::ToString() const
    {
        std::stringstream ss;
        ss << "ExceptNode(constraints: [";
        for (size_t i = 0; i < constraints.size(); ++i)
        {
            ss << "{";
            for (size_t j = 0; j < constraints[i].path.size(); ++j)
            {
                ss << constraints[i].path[j];
                if (j < constraints[i].path.size() - 1) ss << " ";
            }
            ss << "}";
            if (i < constraints.size() - 1) ss << ", ";
        }
        ss << "])";
        return ss.str();
    }

    std::unique_ptr<AstNode> ExceptNode::clone() const
    {
        auto node = std::make_unique<ExceptNode>();
        node->constraints = this->constraints;
        return node;
    }

    void ExceptNode::serialize(std::ostream& os) const
    {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));

        size_t constraint_count = constraints.size();
        os.write(reinterpret_cast<const char*>(&constraint_count), sizeof(constraint_count));
        for (const auto& constraint : constraints)
        {
            os.write(reinterpret_cast<const char*>(&constraint.is_type_constraint), sizeof(constraint.is_type_constraint));
            size_t path_count = constraint.path.size();
            os.write(reinterpret_cast<const char*>(&path_count), sizeof(path_count));
            for (const auto& p : constraint.path)
            {
                size_t len = p.length();
                os.write(reinterpret_cast<const char*>(&len), sizeof(len));
                os.write(p.c_str(), len);
            }
        }
    }

    std::unique_ptr<ExceptNode> ExceptNode::deserialize(std::istream& is)
    {
        auto node = std::make_unique<ExceptNode>();
        size_t constraint_count;
        is.read(reinterpret_cast<char*>(&constraint_count), sizeof(constraint_count));
        node->constraints.resize(constraint_count);
        for (size_t i = 0; i < constraint_count; ++i)
        {
            is.read(reinterpret_cast<char*>(&node->constraints[i].is_type_constraint), sizeof(node->constraints[i].is_type_constraint));
            size_t path_count;
            is.read(reinterpret_cast<char*>(&path_count), sizeof(path_count));
            node->constraints[i].path.resize(path_count);
            for (size_t j = 0; j < path_count; ++j)
            {
                size_t len;
                is.read(reinterpret_cast<char*>(&len), sizeof(len));
                node->constraints[i].path[j].resize(len);
                is.read(&node->constraints[i].path[j][0], len);
            }
        }
        return node;
    }
}