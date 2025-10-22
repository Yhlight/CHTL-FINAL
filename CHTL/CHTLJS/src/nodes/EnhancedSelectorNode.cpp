#include "EnhancedSelectorNode.h"
#include "AstNode.h"
#include "Serialize.h"

namespace CHTLJS
{
    EnhancedSelectorNode::EnhancedSelectorNode(const std::string& selector)
        : selector(selector)
    {
    }

    std::string EnhancedSelectorNode::ToString() const
    {
        return "EnhancedSelectorNode(selector: \"" + selector + "\")";
    }

    std::unique_ptr<AstNode> EnhancedSelectorNode::clone() const
    {
        auto node = std::make_unique<EnhancedSelectorNode>();
        node->selector = this->selector;
        return node;
    }

    void EnhancedSelectorNode::serialize(std::ostream& os) const
    {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));

        CHTL::Serializer::serialize(selector, os);
    }

    std::unique_ptr<EnhancedSelectorNode> EnhancedSelectorNode::deserialize(std::istream& is)
    {
        auto node = std::make_unique<EnhancedSelectorNode>();
        CHTL::Serializer::deserialize(node->selector, is);
        return node;
    }

} // namespace CHTLJS
