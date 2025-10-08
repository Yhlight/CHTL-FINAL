#pragma once

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/RootNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include <string>
#include <sstream>

class Generator : public ASTVisitor {
public:
    std::string generate(RootNode& root);

    void visit(RootNode& node) override;
    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(CommentNode& node) override;
    void visit(StyleNode& node) override;
    void visit(StylePropertyNode& node) override;

private:
    std::stringstream output;
    int indentLevel = 0;
    void indent();
};