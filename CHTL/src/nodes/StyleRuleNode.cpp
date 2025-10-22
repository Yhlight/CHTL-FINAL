#include "StyleRuleNode.h"
#include "StylePropertyNode.h"
#include <sstream>

namespace CHTL
{
    std::string StyleRuleNode::ToString() const
    {
        std::stringstream ss;
        ss << "StyleRuleNode(selector: " << selector << ", properties: [";
        for (size_t i = 0; i < properties.size(); ++i)
        {
            ss << properties[i]->ToString();
            if (i < properties.size() - 1)
            {
                ss << ", ";
            }
        }
        ss << "])";
        return ss.str();
    }

    std::unique_ptr<AstNode> StyleRuleNode::clone() const
    {
        auto node = std::make_unique<StyleRuleNode>();
        node->selector = this->selector;
        for (const auto &prop : this->properties)
        {
            node->properties.push_back(std::unique_ptr<StyleProperty>(static_cast<StyleProperty *>(prop->clone().release())));
        }
        return node;
    }

    void StyleRuleNode::serialize(std::ostream& os) const
    {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));

        size_t len = selector.length();
        os.write(reinterpret_cast<const char*>(&len), sizeof(len));
        os.write(selector.c_str(), len);

        size_t prop_count = properties.size();
        os.write(reinterpret_cast<const char*>(&prop_count), sizeof(prop_count));
        for (const auto& prop : properties)
        {
            prop->serialize(os);
        }
    }

    std::unique_ptr<StyleRuleNode> StyleRuleNode::deserialize(std::istream& is)
    {
        auto node = std::make_unique<StyleRuleNode>();
        size_t len;
        is.read(reinterpret_cast<char*>(&len), sizeof(len));
        node->selector.resize(len);
        is.read(&node->selector[0], len);

        size_t prop_count;
        is.read(reinterpret_cast<char*>(&prop_count), sizeof(prop_count));
        for (size_t i = 0; i < prop_count; ++i)
        {
            node->properties.push_back(std::unique_ptr<StyleProperty>(static_cast<StyleProperty*>(AstNode::deserialize(is).release())));
        }
        return node;
    }
}