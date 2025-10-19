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
}