#pragma once

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/RootNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/StyleRuleNode.h"
#include <vector>
#include <utility>

class StyleCollector : public ASTVisitor {
public:
    std::vector<std::pair<StyleRuleNode*, ElementNode*>> collectedRules;

    void collect(RootNode& root);

private:
    ElementNode* currentElement = nullptr;

    void visit(RootNode& node) override;
    void visit(ElementNode& node) override;
    void visit(StyleNode& node) override;

    // These visitors do nothing in the collection pass
    void visit(TextNode& node) override {}
    void visit(CommentNode& node) override {}
    void visit(StylePropertyNode& node) override {}
    void visit(StyleRuleNode& node) override {}
    void visit(TemplateDeclarationNode& node) override {}
    void visit(TemplateUsageNode& node) override {}
    void visit(CustomDeclarationNode& node) override {}
    void visit(DeleteNode& node) override {}
    void visit(InsertNode& node) override {}
};