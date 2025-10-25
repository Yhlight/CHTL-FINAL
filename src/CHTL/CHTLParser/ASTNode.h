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
    const std::string& getValue() const {
        return value;
    }
    bool getIsString() const {
        return isString;
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
    const std::string& getText() const {
        return text;
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
    const std::string& getKey() const {
        return key;
    }
    const ValueNode* getValue() const {
        return value.get();
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
    const std::string& getName() const {
        return name;
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
    const std::vector<std::unique_ptr<ASTNode>>& getItems() const {
        return items;
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
    const std::string& getName() const {
        return name;
    }
    const StyleNode* getBody() const {
        return body.get();
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
    explicit ElementNode(std::string tag, int index = -1) : tag_name(std::move(tag)), index(index) {}
    std::string toString() const override {
        std::stringstream ss;
        ss << "ElementNode(" << tag_name;
        if (index != -1) {
            ss << "[" << index << "]";
        }
        ss << ", attributes={";
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
    const std::string& getTagName() const {
        return tag_name;
    }
    const std::map<std::string, std::unique_ptr<ValueNode>>& getAttributes() const {
        return attributes;
    }
    const std::vector<std::unique_ptr<ASTNode>>& getChildren() const {
        return children;
    }

private:
    std::string tag_name;
    int index;
    std::map<std::string, std::unique_ptr<ValueNode>> attributes;
    std::vector<std::unique_ptr<ASTNode>> children;
};

class CustomElementNode : public ASTNode {
public:
    CustomElementNode(std::string name, std::vector<std::unique_ptr<ASTNode>> body) : name(std::move(name)), body(std::move(body)) {}
    std::string toString() const override {
        std::stringstream ss;
        ss << "CustomElementNode(" << name << ", {";
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

class CustomElementUsageNode : public ASTNode {
public:
    CustomElementUsageNode(std::string name, std::vector<std::unique_ptr<ASTNode>> body) : name(std::move(name)), body(std::move(body)) {}
    std::string toString() const override {
        std::stringstream ss;
        ss << "CustomElementUsageNode(" << name << ", {";
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
    const std::vector<std::unique_ptr<ASTNode>>& getStatements() const {
        return statements;
    }

private:
    std::vector<std::unique_ptr<ASTNode>> statements;
};

class ImportNode : public ASTNode {
public:
    ImportNode(std::string path, std::string alias) : path(std::move(path)), alias(std::move(alias)) {}
    std::string toString() const override {
        if (alias.empty()) {
            return "ImportNode(\"" + path + "\")";
        }
        return "ImportNode(\"" + path + "\" as " + alias + ")";
    }

private:
    std::string path;
    std::string alias;
};

#endif //CHTL_ASTNODE_H
