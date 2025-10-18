#include "nodes/StylePropertyNode.h"
#include "nodes/ExpressionNode.h"

namespace CHTL
{
    std::string StyleProperty::ToString() const
    {
        return "StyleProperty(" + name + ": " + value->ToString() + ")";
    }
}