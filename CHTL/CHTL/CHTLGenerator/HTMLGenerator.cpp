#include "HTMLGenerator.h"

namespace CHTL {

std::string HTMLGenerator::generate(const ASTNode& root) {
    root.accept(*this);
    return m_output.str();
}

void HTMLGenerator::visit(const TextNode& node) {
    m_output << node.getText();
}

void HTMLGenerator::visit(const ElementNode& node) {
    m_output << "<" << node.getTagName() << ">";
    for (const auto& child : node.getChildren()) {
        child->accept(*this);
    }
    m_output << "</" << node.getTagName() << ">";
}

} // namespace CHTL
