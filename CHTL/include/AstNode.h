#pragma once

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace CHTL
{
    // AST节点类型
    enum class NodeType
    {
        Program,
        Element,
        Text,
        Style,
        Comment,
        StyleProperty,
        StyleRule,
        TemplateDefinition,
        TemplateUsage,
        CustomDefinition,
        CustomUsage,
        DeleteSpecialization,
        Import,
        Namespace,
        Origin,
        Configuration,
        NameConfig,
        // Expressions
        NumberLiteral,
        InfixExpression,
        Identifier,
        ConditionalExpression,
        StringLiteral,
        VariableAccess,
        ExpressionList,
    };

    // AST节点基类
    struct AstNode
    {
        virtual ~AstNode() = default;
        virtual NodeType GetType() const = 0;
        virtual std::string ToString() const = 0;
    };

    // Expression 基类
    struct Expression : public AstNode {};

    // 数字字面量节点
    struct NumberLiteral : public Expression
    {
        double value;
        std::string unit;

        NodeType GetType() const override { return NodeType::NumberLiteral; }
        std::string ToString() const override { return std::to_string(value) + unit; }
    };

    // 中缀表达式节点
    struct InfixExpression : public Expression
    {
        std::unique_ptr<Expression> left;
        std::string op;
        std::unique_ptr<Expression> right;

        NodeType GetType() const override { return NodeType::InfixExpression; }
        std::string ToString() const override {
            return "(" + left->ToString() + " " + op + " " + right->ToString() + ")";
        }
    };

    // 标识符节点
    struct Identifier : public Expression
    {
        std::string value;

        NodeType GetType() const override { return NodeType::Identifier; }
        std::string ToString() const override { return value; }
    };

    // 字符串字面量节点
    struct StringLiteral : public Expression
    {
        std::string value;

        NodeType GetType() const override { return NodeType::StringLiteral; }
        std::string ToString() const override { return "\"" + value + "\""; }
    };

    // 条件表达式节点, e.g., condition ? consequence : alternative
    struct ConditionalExpression : public Expression
    {
        std::unique_ptr<Expression> condition;
        std::unique_ptr<Expression> consequence;
        std::unique_ptr<Expression> alternative;

        NodeType GetType() const override { return NodeType::ConditionalExpression; }
        std::string ToString() const override;
    };

    // 变量访问节点, e.g., TemplateName(VariableName)
    struct VariableAccessNode : public Expression
    {
        std::string template_name;
        std::string variable_name;

        NodeType GetType() const override { return NodeType::VariableAccess; }
        std::string ToString() const override;
    };

    // 表达式列表节点
    struct ExpressionListNode : public Expression
    {
        std::vector<std::unique_ptr<Expression>> expressions;

        NodeType GetType() const override { return NodeType::ExpressionList; }
        std::string ToString() const override;
    };


    // 属性结构体
    struct Attribute
    {
        std::string name;
        std::string value;
    };

    // 元素节点，例如 div { ... }
    struct ElementNode : public AstNode
    {
        std::string tag_name;
        std::vector<Attribute> attributes;
        std::vector<std::unique_ptr<AstNode>> children;

        NodeType GetType() const override { return NodeType::Element; }
        std::string ToString() const override;
    };

    // 文本节点，例如 text { "content" }
    struct TextNode : public AstNode
    {
        std::string value;

        NodeType GetType() const override { return NodeType::Text; }
        std::string ToString() const override { return "TextNode(\"" + value + "\")"; }
    };

    // 样式属性结构体, e.g., width: 100 + 50;
    struct StyleProperty : public AstNode
    {
        std::string name;
        std::unique_ptr<Expression> value;

        NodeType GetType() const override { return NodeType::StyleProperty; } // Need to add StyleProperty to NodeType
        std::string ToString() const override;
    };

    // CSS 规则节点, e.g., .box { width: 100px; }
    struct StyleRuleNode : public AstNode
    {
        std::string selector;
        std::vector<std::unique_ptr<StyleProperty>> properties;

        NodeType GetType() const override { return NodeType::StyleRule; } // Need to add StyleRule to NodeType
        std::string ToString() const override;
    };

    // 样式节点, e.g., style { ... }
    struct StyleNode : public AstNode
    {
        std::vector<std::unique_ptr<AstNode>> children;

        NodeType GetType() const override { return NodeType::Style; }
        std::string ToString() const override;
    };

    // 注释节点
    struct CommentNode : public AstNode
    {
        std::string value;

        NodeType GetType() const override { return NodeType::Comment; }
        std::string ToString() const override { return "# " + value; }
    };

    // 程序根节点
    struct TemplateDefinitionNode; // Forward declaration
    struct CustomDefinitionNode; // Forward declaration

    // 模板使用节点, e.g., @Style DefaultText;
    struct TemplateUsageNode : public AstNode
    {
        std::string type; // e.g., "@Style"
        std::string name;

        NodeType GetType() const override { return NodeType::TemplateUsage; }
        std::string ToString() const override;
    };

    // 自定义使用节点, e.g., @Style MyCustomStyle { ... };
    struct CustomUsageNode : public AstNode
    {
        std::string type; // e.g., "@Style"
        std::string name;
        // Specializations
        std::vector<std::unique_ptr<AstNode>> specializations;

        NodeType GetType() const override { return NodeType::CustomUsage; }
        std::string ToString() const override;
    };

    // 'delete' 特例化节点
    struct DeleteSpecializationNode : public AstNode
    {
        std::string property_name;

        NodeType GetType() const override { return NodeType::DeleteSpecialization; }
        std::string ToString() const override { return "delete " + property_name + ";"; }
    };

    // 导入语句节点, e.g., [Import] @Chtl from "./file.chtl";
    struct ImportNode : public AstNode
    {
        std::string type; // e.g., "@Chtl"
        std::string path;

        NodeType GetType() const override { return NodeType::Import; }
        std::string ToString() const override { return "[Import] " + type + " from \"" + path + "\";"; }
    };

    // 命名空间节点, e.g., [Namespace] MySpace { ... }
    struct NamespaceNode : public AstNode
    {
        std::string name;
        std::vector<std::unique_ptr<AstNode>> children;

        NodeType GetType() const override { return NodeType::Namespace; }
        std::string ToString() const override;
    };

    // 原始嵌入节点, e.g., [Origin] @Html { ... }
    struct OriginNode : public AstNode
    {
        std::string type; // e.g., "@Html", "@Style"
        std::string name; // Optional: e.g., box in [Origin] @Html box { ... }
        std::string content;

        NodeType GetType() const override { return NodeType::Origin; }
        std::string ToString() const override;
    };

    // Name配置块节点, e.g., [Name] { ... }
    struct NameConfigNode : public AstNode
    {
        // Key: CHTL keyword (e.g., "KEYWORD_TEXT"), Value: new name (e.g., "text_element")
        std::unordered_map<std::string, std::string> settings;

        NodeType GetType() const override { return NodeType::NameConfig; }
        std::string ToString() const override;
    };

    // 配置组节点, e.g., [Configuration] { ... }
    struct ConfigurationNode : public AstNode
    {
        std::string name; // Optional: e.g., MyConfig
        // Key: setting name (e.g., "DEBUG_MODE"), Value: setting value (e.g., "true")
        std::unordered_map<std::string, std::string> settings;
        std::unique_ptr<NameConfigNode> name_config;

        NodeType GetType() const override { return NodeType::Configuration; }
        std::string ToString() const override;
    };

    // 模板定义节点, e.g., [Template] @Style DefaultText { ... }
    struct TemplateDefinitionNode : public AstNode
    {
        std::string type; // e.g., "@Style", "@Element"
        std::string name;
        // For @Style or @Var templates
        std::vector<std::unique_ptr<StyleProperty>> properties;
        // For @Element templates
        std::vector<std::unique_ptr<AstNode>> body;

        NodeType GetType() const override { return NodeType::TemplateDefinition; }
        std::string ToString() const override;
    };

    // 自定义定义节点, e.g., [Custom] @Style MyStyle { ... }
    struct CustomDefinitionNode : public AstNode
    {
        std::string type; // e.g., "@Style", "@Element"
        std::string name;
        // Can contain properties for @Style, a body for @Element, and also specializations
        std::vector<std::unique_ptr<AstNode>> children;

        NodeType GetType() const override { return NodeType::CustomDefinition; }
        std::string ToString() const override;
    };

    // Using a nested map for namespaced definitions: namespace -> definition_name -> node
    using TemplateMap = std::unordered_map<std::string, std::unordered_map<std::string, const TemplateDefinitionNode*>>;
    using CustomMap = std::unordered_map<std::string, std::unordered_map<std::string, const CustomDefinitionNode*>>;

    struct ProgramNode : public AstNode
    {
        std::vector<std::unique_ptr<AstNode>> children;
        TemplateMap templates;
        CustomMap customs;
        std::vector<std::unique_ptr<ProgramNode>> imported_programs; // To hold ownership

        NodeType GetType() const override { return NodeType::Program; }
        std::string ToString() const override;
    };

} // namespace CHTL
