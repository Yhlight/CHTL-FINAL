#include "StylePropertyNode.h"
#include "ExpressionNode.h"

namespace CHTL
{
    std::string StyleProperty::ToString() const
    {
        if (value) {
            return "StyleProperty(" + name + ": " + value->ToString() + ")";
        }
        return "StyleProperty(" + name + ")";
    }

    std::unique_ptr<AstNode> StyleProperty::clone() const
    {
        auto node = std::make_unique<StyleProperty>();
        node->name = this->name;
        if (this->value)
        {
            node->value = std::unique_ptr<Expression>(static_cast<Expression*>(this->value->clone().release()));
        }
        return node;
    }

    void StyleProperty::serialize(std::ostream& os) const
    {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));

        size_t len = name.length();
        os.write(reinterpret_cast<const char*>(&len), sizeof(len));
        os.write(name.c_str(), len);

        bool has_value = value != nullptr;
        os.write(reinterpret_cast<const char*>(&has_value), sizeof(has_value));
        if (has_value)
        {
            value->serialize(os);
        }
    }

    std::unique_ptr<StyleProperty> StyleProperty::deserialize(std::istream& is)
    {
        auto node = std::make_unique<StyleProperty>();
        size_t len;
        is.read(reinterpret_cast<char*>(&len), sizeof(len));
        node->name.resize(len);
        is.read(&node->name[0], len);

        bool has_value;
        is.read(reinterpret_cast<char*>(&has_value), sizeof(has_value));
        if (has_value)
        {
            node->value = std::unique_ptr<Expression>(static_cast<Expression*>(AstNode::deserialize(is).release()));
        }
        return node;
    }
}