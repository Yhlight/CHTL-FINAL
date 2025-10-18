#pragma once

#include "AstNode.h"
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL
{
    struct TemplateDefinitionNode; // Forward declaration
    struct CustomDefinitionNode; // Forward declaration

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
}