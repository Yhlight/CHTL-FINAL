#pragma once

#include <memory>
#include <string>
#include <vector>
#include <iostream>

namespace CHTL
{
    // AST节点类型
    enum class NodeType
    {
        Program,
        CMOD,
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
    };

    // AST节点基类
    struct AstNode
    {
        virtual ~AstNode() = default;
        virtual NodeType GetType() const = 0;
        virtual std::string ToString() const = 0;
        virtual std::unique_ptr<AstNode> clone() const = 0;

        virtual void serialize(std::ostream& os) const = 0;
        static std::unique_ptr<AstNode> deserialize(std::istream& is);
    };
}

// 包含所有具体的节点定义
#include "ProgramNode.h"
#include "ElementNode.h"
#include "TextNode.h"
#include "StyleNode.h"
#include "CommentNode.h"
#include "ScriptNode.h"
#include "OriginNode.h"
#include "ImportNode.h"
#include "NamespaceNode.h"
#include "ConfigurationNode.h"
#include "NameConfigNode.h"
#include "UseNode.h"
#include "ExceptNode.h"
#include "TemplateDefinitionNode.h"
#include "CustomDefinitionNode.h"
#include "TemplateUsageNode.h"
#include "CustomUsageNode.h"
#include "DeleteSpecializationNode.h"
#include "InsertSpecializationNode.h"
#include "IfNode.h"
#include "ElseNode.h"
#include "StylePropertyNode.h"
#include "StyleRuleNode.h"
#include "ExpressionNode.h"