#pragma once

#include "AstNode.h"
#include <string>

namespace CHTL
{
    // 'delete' 特例化节点
    struct DeleteSpecializationNode : public AstNode
    {
        std::string property_name;

        NodeType GetType() const override { return NodeType::DeleteSpecialization; }
        std::string ToString() const override { return "delete " + property_name + ";"; }
    };
}