#ifndef CHTML_EXPORTNODE_H
#define CHTML_EXPORTNODE_H

#include "AstNode.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>

namespace CHTL
{
    struct ExportItem
    {
        std::string type; // e.g., "[Custom] @Style"
        std::vector<std::string> names;
    };

    class ExportNode : public AstNode
    {
    public:
        std::vector<ExportItem> items;

        ExportNode() = default;

        NodeType GetType() const override { return NodeType::Export; }
        std::string ToString() const override {
            std::stringstream ss;
            ss << "[Export] {\n";
            for (const auto& item : items) {
                ss << "    " << item.type << " ";
                for (size_t i = 0; i < item.names.size(); ++i) {
                    ss << item.names[i] << (i < item.names.size() - 1 ? ", " : "");
                }
                ss << ";\n";
            }
            ss << "}";
            return ss.str();
        }
        void serialize(std::ostream& os) const override {
            // No-op for now
        }

        virtual std::unique_ptr<AstNode> clone() const override
        {
            auto node = std::make_unique<ExportNode>();
            node->items = this->items;
            return node;
        }
    };
}

#endif //CHTML_EXPORTNODE_H