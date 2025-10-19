#include "TemplateUsageNode.h"

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
}