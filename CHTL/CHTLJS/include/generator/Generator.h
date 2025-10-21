#pragma once

#include "CHTLJS/include/nodes/AstNode.h"
#include "CHTLJS/include/nodes/EnhancedSelectorNode.h"
#include "CHTLJS/include/nodes/RawJSNode.h"
#include "bridge/SaltBridge.h"
#include <string>
#include <memory>

namespace CHTLJS {

class Generator {
public:
    Generator(std::shared_ptr<CHTL::SaltBridge> bridge);
    std::string Generate(const ProgramNode& program);

private:
    void visit(const AstNode& node);
    // Add visit methods for each node type here...
    void visit(const EnhancedSelectorNode& node);
    void visit(const RawJSNode& node);

    std::shared_ptr<CHTL::SaltBridge> bridge_;
    std::string output_;
};

}
