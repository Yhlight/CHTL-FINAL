#pragma once

#include "AstNode.h"
#include <vector>
#include <string>

namespace CHTL
{
    struct ExceptNode : public AstNode
    {
        struct Constraint {
            bool is_type_constraint; // true if it's like [Custom] or @Html
            std::vector<std::string> path; // e.g., {"span"} or {"[Custom]", "@Element", "Box"} or {"@Html"}
        };
        std::vector<Constraint> constraints;

        NodeType GetType() const override { return NodeType::Except; }
        std::string ToString() const override;
        std::unique_ptr<AstNode> clone() const override;

        void serialize(std::ostream& os) const override;
        static std::unique_ptr<ExceptNode> deserialize(std::istream& is);
    };
}