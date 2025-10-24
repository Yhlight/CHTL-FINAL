#ifndef CHTL_HTML_GENERATOR_H
#define CHTL_HTML_GENERATOR_H

#include "../CHTLNode/ASTNode.h"
#include <string>
#include <sstream>

namespace CHTL {

class HTMLGenerator : public ASTVisitor {
public:
    std::string generate(const ASTNode& root);

    void visit(const TextNode& node) override;
    void visit(const ElementNode& node) override;
    void visit(const StyleNode& node) override;

private:
    std::stringstream m_output;
};

} // namespace CHTL

#endif // CHTL_HTML_GENERATOR_H
