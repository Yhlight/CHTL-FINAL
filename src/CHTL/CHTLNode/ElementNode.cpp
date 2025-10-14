#include "ElementNode.h"
#include "NodeVisitor.h"

namespace CHTL {

void ElementNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
