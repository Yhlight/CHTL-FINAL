#pragma once

#include "AstNode.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace CHTLJS
{
    struct DelegateNode : public AstNode
    {
        std::unique_ptr<AstNode> parent_target;
        std::vector<std::string> child_targets;
        std::unordered_map<std::string, std::string> events;
        NodeType GetType() const override { return NodeType::Delegate; }
        std::string ToString() const override { return "DelegateNode"; }
        std::unique_ptr<AstNode> clone() const override;
        void serialize(std::ostream& os) const override;
        static std::unique_ptr<DelegateNode> deserialize(std::istream& is);
    };

}
