#include "TextNode.h"
#include "NodeVisitor.h"

namespace CHTL {

void TextNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
