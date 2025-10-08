#pragma once

#include <memory>
#include <vector>
#include <string>

// Forward declarations for node types
class RootNode;
class ElementNode;
class TextNode;
class CommentNode;
class StyleNode;
class StylePropertyNode;
class StyleRuleNode;
class TemplateDeclarationNode;
class TemplateUsageNode;
class CustomDeclarationNode;
class DeleteNode;
class InsertNode;

// Visitor pattern for traversing the AST
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(RootNode& node) = 0;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(CommentNode& node) = 0;
    virtual void visit(StyleNode& node) = 0;
    virtual void visit(StylePropertyNode& node) = 0;
    virtual void visit(StyleRuleNode& node) = 0;
    virtual void visit(TemplateDeclarationNode& node) = 0;
    virtual void visit(TemplateUsageNode& node) = 0;
    virtual void visit(CustomDeclarationNode& node) = 0;
    virtual void visit(DeleteNode& node) = 0;
    virtual void visit(InsertNode& node) = 0;
};

class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
    virtual std::unique_ptr<BaseNode> clone() const = 0;
};