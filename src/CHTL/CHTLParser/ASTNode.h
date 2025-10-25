#ifndef CHTL_ASTNODE_H
#define CHTL_ASTNODE_H

#include <string>
#include <utility>
#include <vector>
#include <memory>
#include <sstream>
#include <map>

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual std::string toString() const = 0;
};

class TextNode : public ASTNode {
public:
    explicit TextNode(std::string text) : text(std::move(text)) {}
    std::string toString() const override {
        return "TextNode(\"" + text + "\")";
    }

private:
    std::string text;
};

class ElementNode : public ASTNode {
public:
    explicit ElementNode(std::string tag) : tag_name(std::move(tag)) {}
    std::string toString() const override {
        std::stringstream ss;
        ss << "ElementNode(" << tag_name << ", attributes={";
        for (const auto& attr : attributes) {
            ss << attr.first << ": \"" << attr.second << "\", ";
        }
        ss << "}, children={";
        for (const auto& child : children) {
            ss << child->toString() << ", ";
        }
        ss << "})";
        return ss.str();
    }
    void addChild(std::unique_ptr<ASTNode> child) {
        children.push_back(std::move(child));
    }
    void addAttribute(const std::string& key, const std::string& value) {
        attributes[key] = value;
    }

private:
    std::string tag_name;
    std::map<std::string, std::string> attributes;
    std::vector<std::unique_ptr<ASTNode>> children;
};

class ProgramNode : public ASTNode {
public:
    ProgramNode() = default;
    std::string toString() const override {
        std::stringstream ss;
        ss << "ProgramNode({";
        for (const auto& stmt : statements) {
            ss << stmt->toString() << ", ";
        }
        ss << "})";
        return ss.str();
    }
    void addStatement(std::unique_ptr<ASTNode> stmt) {
        statements.push_back(std::move(stmt));
    }

private:
    std::vector<std::unique_ptr<ASTNode>> statements;
};

#endif //CHTL_ASTNODE_H
