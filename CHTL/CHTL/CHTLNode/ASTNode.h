#ifndef CHTL_AST_NODE_H
#define CHTL_AST_NODE_H

#include <memory>
#include <string>
#include <vector>
#include "../CHTLLexer/Token.h"

namespace CHTL {

class TextNode;
class ElementNode;

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(const TextNode& node) = 0;
    virtual void visit(const ElementNode& node) = 0;
};

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) const = 0;
};

class TextNode : public ASTNode {
public:
    explicit TextNode(const std::string& text) : m_text(text) {}

    const std::string& getText() const { return m_text; }

    void accept(ASTVisitor& visitor) const override {
        visitor.visit(*this);
    }

private:
    std::string m_text;
};

class ElementNode : public ASTNode {
public:
    ElementNode(std::string tagName, std::vector<std::unique_ptr<ASTNode>> children)
        : m_tagName(std::move(tagName)), m_children(std::move(children)) {}

    const std::string& getTagName() const { return m_tagName; }
    const std::vector<std::unique_ptr<ASTNode>>& getChildren() const { return m_children; }

    void accept(ASTVisitor& visitor) const override {
        visitor.visit(*this);
    }

private:
    std::string m_tagName;
    std::vector<std::unique_ptr<ASTNode>> m_children;
};

} // namespace CHTL

#endif // CHTL_AST_NODE_H
