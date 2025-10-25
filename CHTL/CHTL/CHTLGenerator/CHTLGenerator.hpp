#ifndef CHTL_GENERATOR_HPP
#define CHTL_GENERATOR_HPP

#include "CHTLNode/AstNode.hpp"
#include "CHTLNode/ElementNode.hpp"
#include "CHTLNode/TextNode.hpp"
#include <string>

class CHTLGenerator {
public:
    std::string generate(const AstNode& root);

private:
    void visit(const AstNode& node);
    void visitElementNode(const ElementNode& node);
    void visitTextNode(const TextNode& node);

    std::string html_;
};

#endif // CHTL_GENERATOR_HPP
