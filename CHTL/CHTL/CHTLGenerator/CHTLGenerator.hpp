#ifndef CHTL_GENERATOR_HPP
#define CHTL_GENERATOR_HPP

#include "CHTLNode/AstNode.hpp"
#include "CHTLNode/ElementNode.hpp"
#include "CHTLNode/TextNode.hpp"
#include "CHTLNode/RuleNode.hpp"
#include <string>
#include "Document.hpp"

class CHTLGenerator {
public:
    Document generate(const AstNode& root);

private:
    void visit(const AstNode& node);
    void visitElementNode(const ElementNode& node);
    void visitTextNode(const TextNode& node);
    void visitRuleNode(const RuleNode& node);

    std::string html_;
    std::string css_;
};

#endif // CHTL_GENERATOR_HPP
