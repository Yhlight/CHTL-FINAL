#pragma once

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/RootNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/StyleRuleNode.h"
#include <set>
#include <sstream>

class AttributeInjector : public ASTVisitor {
public:
    void inject(RootNode& root);

private:
    void visit(RootNode& node) override;
    void visit(ElementNode& node) override;

    // These visitors do nothing in this pass
    void visit(TextNode& node) override {}
    void visit(CommentNode& node) override {}
    void visit(StyleNode& node) override {}
    void visit(StylePropertyNode& node) override {}
    void visit(StyleRuleNode& node) override {}
    void visit(TemplateDeclarationNode& node) override {}
    void visit(TemplateUsageNode& node) override {}
};