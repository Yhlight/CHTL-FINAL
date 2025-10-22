#pragma once

#include <memory>
#include <string>
#include "Config.h"

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
        Script,
        Origin,
        Import,
        Namespace,
        Configuration,
        NameConfig,
        Use,
        Except,
        // Definitions
        TemplateDefinition,
        CustomDefinition,
        // Usages
        TemplateUsage,
        CustomUsage,
        // Specializations
        DeleteSpecialization,
        InsertSpecialization,
        If,
        Else,
        // Style-related
        StyleProperty,
        StyleRule,
        // Expressions
        NumberLiteral,
        StringLiteral,
        Identifier,
        InfixExpression,
        ConditionalExpression,
        VariableAccess,
        AttributeAccess,
        ExpressionList,
        ReactiveValue,
    };

    // AST节点基类
    struct AstNode
    {
        virtual ~AstNode() = default;
        virtual NodeType GetType() const = 0;
        virtual std::string ToString() const = 0;
        virtual std::unique_ptr<AstNode> clone() const = 0;
    };
}
