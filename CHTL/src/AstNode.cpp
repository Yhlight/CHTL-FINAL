#include "AstNode.h"
#include "ProgramNode.h"
#include "ElementNode.h"
#include "TextNode.h"
#include "CommentNode.h"
#include "ConfigurationNode.h"
#include "NameConfigNode.h"
#include "CustomDefinitionNode.h"
#include "CustomUsageNode.h"
#include "DeleteSpecializationNode.h"
#include "ElseNode.h"
#include "ExceptNode.h"
#include "IfNode.h"
#include "ImportNode.h"
#include "InsertSpecializationNode.h"
#include "NamespaceNode.h"
#include "OriginNode.h"
#include "ScriptNode.h"
#include "StyleNode.h"
#include "StyleRuleNode.h"
#include "StylePropertyNode.h"
#include "TemplateDefinitionNode.h"
#include "TemplateUsageNode.h"
#include "UseNode.h"
#include "nodes/ExpressionNode.h"
#include <stdexcept>
#include <string>
#include <iostream>

namespace CHTL
{
    std::unique_ptr<AstNode> AstNode::deserialize(std::istream& is)
    {
        int type;
        is.read(reinterpret_cast<char*>(&type), sizeof(type));

        if (is.gcount() == 0)
        {
            return nullptr;
        }

        switch (static_cast<NodeType>(type))
        {
        case NodeType::Program:
            return ProgramNode::deserialize(is);
        case NodeType::Element:
            return ElementNode::deserialize(is);
        case NodeType::Text:
            return TextNode::deserialize(is);
        case NodeType::Comment:
            return CommentNode::deserialize(is);
        case NodeType::Configuration:
            return ConfigurationNode::deserialize(is);
        case NodeType::NameConfig:
            return NameConfigNode::deserialize(is);
        case NodeType::CustomDefinition:
            return CustomDefinitionNode::deserialize(is);
        case NodeType::CustomUsage:
            return CustomUsageNode::deserialize(is);
        case NodeType::DeleteSpecialization:
            return DeleteSpecializationNode::deserialize(is);
        case NodeType::Else:
            return ElseNode::deserialize(is);
        case NodeType::Except:
            return ExceptNode::deserialize(is);
        case NodeType::If:
            return IfNode::deserialize(is);
        case NodeType::Import:
            return ImportNode::deserialize(is);
        case NodeType::InsertSpecialization:
            return InsertSpecializationNode::deserialize(is);
        case NodeType::Namespace:
            return NamespaceNode::deserialize(is);
        case NodeType::Origin:
            return OriginNode::deserialize(is);
        case NodeType::Script:
            return ScriptNode::deserialize(is);
        case NodeType::Style:
            return StyleNode::deserialize(is);
        case NodeType::StyleRule:
            return StyleRuleNode::deserialize(is);
        case NodeType::StyleProperty:
            return StyleProperty::deserialize(is);
        case NodeType::TemplateDefinition:
            return TemplateDefinitionNode::deserialize(is);
        case NodeType::TemplateUsage:
            return TemplateUsageNode::deserialize(is);
        case NodeType::Use:
            return UseNode::deserialize(is);
        case NodeType::NumberLiteral:
            return NumberLiteral::deserialize(is);
        default:
            throw std::runtime_error("Unknown node type: " + std::to_string(type));
        }
    }
}
