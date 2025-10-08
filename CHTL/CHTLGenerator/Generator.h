#pragma once

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/RootNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "StyleCollector.h"
#include "AttributeInjector.h"
#include "ExpressionEvaluator.h"
#include "../CHTLContext/CHTLContext.h"
#include <string>
#include <sstream>
#include <vector>

class Generator : public ASTVisitor {
public:
    std::string generate(RootNode& root, CHTLContext& context);

    void visit(RootNode& node) override;
    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(CommentNode& node) override;
    void visit(StyleNode& node) override;
    void visit(StylePropertyNode& node) override;
    void visit(StyleRuleNode& node) override;
    void visit(TemplateDeclarationNode& node) override;
    void visit(TemplateUsageNode& node) override;

private:
    CHTLContext* context = nullptr;
    std::stringstream output;
    std::vector<std::pair<StyleRuleNode*, ElementNode*>> collectedRules;
    int indentLevel = 0;
    void indent();
};