#include "BaseNode.h"

namespace CHTL {

const char* nodeTypeToString(NodeType type) {
    switch (type) {
        case NodeType::ELEMENT: return "ELEMENT";
        case NodeType::TEXT: return "TEXT";
        case NodeType::ATTRIBUTE: return "ATTRIBUTE";
        case NodeType::STYLE_BLOCK: return "STYLE_BLOCK";
        case NodeType::SCRIPT_BLOCK: return "SCRIPT_BLOCK";
        case NodeType::TEMPLATE: return "TEMPLATE";
        case NodeType::CUSTOM: return "CUSTOM";
        case NodeType::IMPORT: return "IMPORT";
        case NodeType::NAMESPACE: return "NAMESPACE";
        case NodeType::COMMENT: return "COMMENT";
        default: return "UNKNOWN";
    }
}

} // namespace CHTL
