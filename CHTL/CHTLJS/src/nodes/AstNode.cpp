#include "AstNode.h"
#include "ProgramNode.h"
#include "AnimateNode.h"
#include "ListenNode.h"
#include "DelegateNode.h"
#include "EnhancedSelectorNode.h"
#include "RawJSNode.h"
#include "ScriptLoaderNode.h"
#include "VirNode.h"
#include <stdexcept>

namespace CHTLJS
{
    std::unique_ptr<AstNode> AstNode::deserialize(std::istream& is)
    {
        int type;
        is.read(reinterpret_cast<char*>(&type), sizeof(type));

        if (is.gcount() == 0)
        {
            return nullptr;
        }

        switch (static_cast<NodeType>(type))
        {
            case NodeType::Program:
                return ProgramNode::deserialize(is);
            case NodeType::Animate:
                return AnimateNode::deserialize(is);
            case NodeType::Listen:
                return ListenNode::deserialize(is);
            case NodeType::Delegate:
                return DelegateNode::deserialize(is);
            case NodeType::EnhancedSelector:
                return EnhancedSelectorNode::deserialize(is);
            case NodeType::RawJS:
                return RawJSNode::deserialize(is);
            case NodeType::ScriptLoader:
                return ScriptLoaderNode::deserialize(is);
            case NodeType::Vir:
                return VirNode::deserialize(is);
            default:
                throw std::runtime_error("Unknown CHTLJS node type for deserialization");
        }
    }
}
