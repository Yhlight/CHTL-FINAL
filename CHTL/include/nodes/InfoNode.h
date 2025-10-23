#ifndef CHTML_INFONODE_H
#define CHTML_INFONODE_H

#include "AstNode.h"
#include <string>
#include <unordered_map>
#include <sstream>

namespace CHTL
{
    class InfoNode : public AstNode
    {
    public:
        std::unordered_map<std::string, std::string> metadata;

        InfoNode() = default;

        NodeType GetType() const override { return NodeType::Info; }
        std::string ToString() const override {
            std::stringstream ss;
            ss << "[Info] {\n";
            for (const auto& pair : metadata) {
                ss << "    " << pair.first << ": \"" << pair.second << "\";\n";
            }
            ss << "}";
            return ss.str();
        }
        void serialize(std::ostream& os) const override {
            // No-op for now
        }

        virtual std::unique_ptr<AstNode> clone() const override
        {
            auto node = std::make_unique<InfoNode>();
            node->metadata = this->metadata;
            return node;
        }
    };
}

#endif //CHTML_INFONODE_H