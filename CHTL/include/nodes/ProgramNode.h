#pragma once

#include "AstNode.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include "CHTLJS/include/nodes/ProgramNode.h"


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
        std::vector<std::unique_ptr<CHTLJS::ProgramNode>> cjs_nodes;
        TemplateMap templates;
        CustomMap customs;

        NodeType GetType() const override { return NodeType::Program; }
        std::string ToString() const override;
        std::unique_ptr<AstNode> clone() const override;
    };
}