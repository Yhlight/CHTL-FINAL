#include "TemplateUsageNode.h"
#include "Serialize.h"

namespace CHTL
{
    std::string TemplateUsageNode::ToString() const
    {
        return "TemplateUsage(" + type + " " + name + ")";
    }

    std::unique_ptr<AstNode> TemplateUsageNode::clone() const
    {
        auto node = std::make_unique<TemplateUsageNode>();
        node->type = this->type;
        node->name = this->name;
        return node;
    }

    void TemplateUsageNode::serialize(std::ostream& os) const
    {
        int node_type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&node_type), sizeof(node_type));
        Serializer::serialize(type, os);
        Serializer::serialize(name, os);
    }

    std::unique_ptr<TemplateUsageNode> TemplateUsageNode::deserialize(std::istream& is)
    {
        auto node = std::make_unique<TemplateUsageNode>();
        Serializer::deserialize(node->type, is);
        Serializer::deserialize(node->name, is);
        return node;
    }
}