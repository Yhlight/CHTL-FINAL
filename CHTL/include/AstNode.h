#pragma once

#include <memory>
#include <string>

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
        ExpressionList,
    };

    // AST节点基类
    struct AstNode
    {
        virtual ~AstNode() = default;
        virtual NodeType GetType() const = 0;
        virtual std::string ToString() const = 0;
    };
}

// 包含所有具体的节点定义
#include "nodes/ProgramNode.h"
#include "nodes/ElementNode.h"
#include "nodes/TextNode.h"
#include "nodes/StyleNode.h"
#include "nodes/CommentNode.h"
#include "nodes/ScriptNode.h"
#include "nodes/OriginNode.h"
#include "nodes/ImportNode.h"
#include "nodes/NamespaceNode.h"
#include "nodes/ConfigurationNode.h"
#include "nodes/NameConfigNode.h"
#include "nodes/UseNode.h"
#include "nodes/ExceptNode.h"
#include "nodes/TemplateDefinitionNode.h"
#include "nodes/CustomDefinitionNode.h"
#include "nodes/TemplateUsageNode.h"
#include "nodes/CustomUsageNode.h"
#include "nodes/DeleteSpecializationNode.h"
#include "nodes/InsertSpecializationNode.h"
#include "nodes/StylePropertyNode.h"
#include "nodes/StyleRuleNode.h"
#include "nodes/ExpressionNode.h"