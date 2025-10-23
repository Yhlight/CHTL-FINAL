#pragma once

#include "AstNode.h"
#include <vector>
#include <string>

namespace CHTL
{
    class UseNode : public AstNode
    {
    public:
        std::vector<std::string> path;

        NodeType GetType() const override { return NodeType::Use; }

        std::string ToString() const override;
        std::unique_ptr<AstNode> clone() const override;
        void serialize(std::ostream& os) const override;
    };
}