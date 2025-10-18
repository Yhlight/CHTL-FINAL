#include "StylePropertyNode.h"
#include "ExpressionNode.h"

namespace CHTL
{
    std::string StyleProperty::ToString() const
    {
        return "StyleProperty(" + name + ": " + value->ToString() + ")";
    }
}