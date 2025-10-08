#pragma once

#include <memory>
#include <vector>
#include <string>

// Forward declarations for node types
class RootNode;
class ElementNode;
class TextNode;
class CommentNode;

// Visitor pattern for traversing the AST
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(RootNode& node) = 0;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(CommentNode& node) = 0;
};

class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
};