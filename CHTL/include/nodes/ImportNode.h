#pragma once

#include "AstNode.h"
#include <string>

namespace CHTL
{
    struct ImportNode : public AstNode
    {
        // For通配导入, e.g., [Import] @Chtl
        std::string type;

        // For 精确导入 or 类型导入
        std::string import_scope; // e.g., "[Template]", "[Custom]"
        std::string specific_type; // e.g., "@Style", "@Element"
        std::string imported_name; // The specific name to import, empty for 类型导入
        std::string alias; // The new name given with 'as'

        std::string path;

        NodeType GetType() const override { return NodeType::Import; }
        std::string ToString() const override; // Implementation in .cpp file
        std::unique_ptr<AstNode> clone() const override;

        void serialize(std::ostream& os) const override;
        static std::unique_ptr<ImportNode> deserialize(std::istream& is);
    };
}