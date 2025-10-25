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

class ValueNode : public ASTNode {};

class LiteralValueNode : public ValueNode {
public:
    explicit LiteralValueNode(std::string value, bool isString = false) : value(std::move(value)), isString(isString) {}
    std::string toString() const override {
        if (isString) {
            return "LiteralValueNode(\"" + value + "\")";
        }
        return "LiteralValueNode(" + value + ")";
    }

private:
    std::string value;
    bool isString;
};

class TemplateVarUsageNode : public ValueNode {
public:
    TemplateVarUsageNode(std::string templateName, std::string varName) : templateName(std::move(templateName)), varName(std::move(varName)) {}
    std::string toString() const override {
        return "TemplateVarUsageNode(" + templateName + "(" + varName + "))";
    }

private:
    std::string templateName;
    std::string varName;
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

class StylePropertyNode : public ASTNode {
public:
    StylePropertyNode(std::string key, std::unique_ptr<ValueNode> value) : key(std::move(key)), value(std::move(value)) {}
    std::string toString() const override {
        if (value) {
            return "StylePropertyNode(" + key + ": " + value->toString() + ")";
        }
        return "StylePropertyNode(" + key + ")";
    }

private:
    std::string key;
    std::unique_ptr<ValueNode> value;
};

class StyleTemplateUsageNode : public ASTNode {
public:
    explicit StyleTemplateUsageNode(std::string name) : name(std::move(name)) {}
    std::string toString() const override {
        return "StyleTemplateUsageNode(" + name + ")";
    }

private:
    std::string name;
};

class CustomStyleUsageNode : public ASTNode {
public:
    CustomStyleUsageNode(std::string name, std::unique_ptr<ASTNode> body) : name(std::move(name)), body(std::move(body)) {}
    std::string toString() const override {
        return "CustomStyleUsageNode(" + name + ", " + body->toString() + ")";
    }

private:
    std::string name;
    std::unique_ptr<ASTNode> body;
};

class ElementTemplateUsageNode : public ASTNode {
public:
    explicit ElementTemplateUsageNode(std::string name) : name(std::move(name)) {}
    std::string toString() const override {
        return "ElementTemplateUsageNode(" + name + ")";
    }

private:
    std::string name;
};

class StyleNode : public ASTNode {
public:
    StyleNode() = default;
    std::string toString() const override {
        std::stringstream ss;
        ss << "StyleNode({";
        for (const auto& item : items) {
            ss << item->toString() << ", ";
        }
        ss << "})";
        return ss.str();
    }
    void addItem(std::unique_ptr<ASTNode> item) {
        items.push_back(std::move(item));
    }

private:
    std::vector<std::unique_ptr<ASTNode>> items;
};

class StyleTemplateNode : public ASTNode {
public:
    StyleTemplateNode(std::string name, std::unique_ptr<StyleNode> body) : name(std::move(name)), body(std::move(body)) {}
    std::string toString() const override {
        return "StyleTemplateNode(" + name + ", " + body->toString() + ")";
    }

private:
    std::string name;
    std::unique_ptr<StyleNode> body;
};

class CustomStyleTemplateNode : public ASTNode {
public:
    CustomStyleTemplateNode(std::string name, std::unique_ptr<StyleNode> body) : name(std::move(name)), body(std::move(body)) {}
    std::string toString() const override {
        return "CustomStyleTemplateNode(" + name + ", " + body->toString() + ")";
    }

private:
    std::string name;
    std::unique_ptr<StyleNode> body;
};

class VarTemplateNode : public ASTNode {
public:
    explicit VarTemplateNode(std::string name) : name(std::move(name)) {}
    std::string toString() const override {
        std::stringstream ss;
        ss << "VarTemplateNode(" << name << ", {";
        for (const auto& var : variables) {
            ss << var.first << ": " << var.second->toString() << ", ";
        }
        ss << "})";
        return ss.str();
    }
    void addVariable(const std::string& key, std::unique_ptr<ValueNode> value) {
        variables[key] = std::move(value);
    }

private:
    std::string name;
    std::map<std::string, std::unique_ptr<ValueNode>> variables;
};

class ElementNode : public ASTNode {
public:
    explicit ElementNode(std::string tag) : tag_name(std::move(tag)) {}
    std::string toString() const override {
        std::stringstream ss;
        ss << "ElementNode(" << tag_name << ", attributes={";
        for (const auto& attr : attributes) {
            ss << attr.first << ": " << attr.second->toString() << ", ";
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
    void addAttribute(const std::string& key, std::unique_ptr<ValueNode> value) {
        attributes[key] = std::move(value);
    }

private:
    std::string tag_name;
    std::map<std::string, std::unique_ptr<ValueNode>> attributes;
    std::vector<std::unique_ptr<ASTNode>> children;
};

class ElementTemplateNode : public ASTNode {
public:
    ElementTemplateNode(std::string name, std::vector<std::unique_ptr<ASTNode>> body) : name(std::move(name)), body(std::move(body)) {}
    std::string toString() const override {
        std::stringstream ss;
        ss << "ElementTemplateNode(" << name << ", {";
        for (const auto& child : body) {
            ss << child->toString() << ", ";
        }
        ss << "})";
        return ss.str();
    }

private:
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> body;
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
