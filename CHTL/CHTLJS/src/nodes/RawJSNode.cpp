#include "CHTLJS/include/nodes/RawJSNode.h"

namespace CHTLJS
{
    std::string RawJSNode::ToString() const
    {
        return "RawJSNode(content: \"" + content + "\")";
    }

} // namespace CHTLJS
