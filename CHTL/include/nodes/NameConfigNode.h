#pragma once

#include "AstNode.h"
#include <string>
#include <unordered_map>

namespace CHTL
{
    // Name配置块节点, e.g., [Name] { ... }
    struct NameConfigNode : public AstNode
    {
        // Key: CHTL keyword (e.g., "KEYWORD_TEXT"), Value: new name (e.g., "text_element")
        std::unordered_map<std::string, std::string> settings;

        NodeType GetType() const override { return NodeType::NameConfig; }
        std::string ToString() const override;
        std::unique_ptr<AstNode> clone() const override;

        void serialize(std::ostream& os) const override;
        static std::unique_ptr<NameConfigNode> deserialize(std::istream& is);
    };
}