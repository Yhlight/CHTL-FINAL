#include "nodes/DeleteSpecializationNode.h"

namespace CHTL
{
    std::unique_ptr<AstNode> DeleteSpecializationNode::clone() const
    {
        auto node = std::make_unique<DeleteSpecializationNode>();
        node->property_name = this->property_name;
        return node;
    }

    void DeleteSpecializationNode::serialize(std::ostream& os) const
    {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));

        size_t len = property_name.length();
        os.write(reinterpret_cast<const char*>(&len), sizeof(len));
        os.write(property_name.c_str(), len);
    }

    std::unique_ptr<DeleteSpecializationNode> DeleteSpecializationNode::deserialize(std::istream& is)
    {
        auto node = std::make_unique<DeleteSpecializationNode>();
        size_t len;
        is.read(reinterpret_cast<char*>(&len), sizeof(len));
        node->property_name.resize(len);
        is.read(&node->property_name[0], len);
        return node;
    }
}
