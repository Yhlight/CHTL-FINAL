#ifndef CHTL_AST_NODE_H
#define CHTL_AST_NODE_H

#include <memory>
#include <string>
#include <vector>
#include <variant>
#include "../CHTLLexer/Token.h"

namespace CHTL {

class TextNode;
class ElementNode;
class StyleNode;

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(const TextNode& node) = 0;
    virtual void visit(const ElementNode& node) = 0;
    virtual void visit(const StyleNode& node) = 0;
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

struct Attribute {
    std::string key;
    std::string value;
};

struct Selector {
    enum class Type { Class, Id, Context };
    Type type;
    std::string name;
    std::vector<Attribute> properties;
};

class ElementNode : public ASTNode {
public:
    ElementNode(std::string tagName, std::vector<Attribute> attributes, std::vector<std::unique_ptr<ASTNode>> children)
        : m_tagName(std::move(tagName)), m_attributes(std::move(attributes)), m_children(std::move(children)) {}

    const std::string& getTagName() const { return m_tagName; }
    const std::vector<Attribute>& getAttributes() const { return m_attributes; }
    const std::vector<std::unique_ptr<ASTNode>>& getChildren() const { return m_children; }

    void accept(ASTVisitor& visitor) const override {
        visitor.visit(*this);
    }

private:
    std::string m_tagName;
    std::vector<Attribute> m_attributes;
    std::vector<std::unique_ptr<ASTNode>> m_children;
};

class StyleNode : public ASTNode {
public:
    explicit StyleNode(std::vector<Attribute> inline_properties, std::vector<Selector> selectors)
    : m_inline_properties(std::move(inline_properties)), m_selectors(std::move(selectors)) {}

    const std::vector<Attribute>& getInlineProperties() const { return m_inline_properties; }
    const std::vector<Selector>& getSelectors() const { return m_selectors; }

    void accept(ASTVisitor& visitor) const override {
        visitor.visit(*this);
    }

private:
    std::vector<Attribute> m_inline_properties;
    std::vector<Selector> m_selectors;
};

} // namespace CHTL

#endif // CHTL_AST_NODE_H
