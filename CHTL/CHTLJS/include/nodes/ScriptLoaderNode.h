#pragma once

#include "AstNode.h"
#include <vector>
#include <string>

namespace CHTLJS
{
    struct ScriptLoaderNode : public AstNode
    {
        std::vector<std::string> paths;
        NodeType GetType() const override { return NodeType::ScriptLoader; }
        std::string ToString() const override { return "ScriptLoaderNode"; }
        std::unique_ptr<AstNode> clone() const override;
        void serialize(std::ostream& os) const override;
        static std::unique_ptr<ScriptLoaderNode> deserialize(std::istream& is);
    };

}
