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

public:
    std::string getGlobalCSS() const { return m_global_css.str(); }

private:
    std::stringstream m_output;
    std::stringstream m_global_css;
};

} // namespace CHTL

#endif // CHTL_HTML_GENERATOR_H
