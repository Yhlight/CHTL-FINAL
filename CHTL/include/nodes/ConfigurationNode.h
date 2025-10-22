#pragma once

#include "AstNode.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace CHTL
{
    struct NameConfigNode; // Forward declaration

    struct ConfigurationNode : public AstNode
    {
        std::string name; // Optional: e.g., MyConfig
        // Key: setting name (e.g., "DEBUG_MODE"), Value: setting value (e.g., "true")
        std::unordered_map<std::string, std::string> settings;
        std::unique_ptr<NameConfigNode> name_config;

        NodeType GetType() const override { return NodeType::Configuration; }
        std::string ToString() const override;
        std::unique_ptr<AstNode> clone() const override;

        void serialize(std::ostream& os) const override;
        static std::unique_ptr<ConfigurationNode> deserialize(std::istream& is);
    };
}