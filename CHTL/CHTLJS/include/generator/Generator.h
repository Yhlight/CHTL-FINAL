#pragma once

#include "CHTLJS/include/nodes/AstNode.h"
#include "CHTLJS/include/nodes/EnhancedSelectorNode.h"
#include "CHTLJS/include/nodes/RawJSNode.h"
#include "CHTLJS/include/nodes/ScriptLoaderNode.h"
#include "CHTLJS/include/nodes/ListenNode.h"
#include "CHTLJS/include/nodes/DelegateNode.h"
#include "CHTLJS/include/nodes/AnimateNode.h"
#include "CHTLJS/include/nodes/VirNode.h"
#include "bridge/SaltBridge.h"
#include <string>
#include <memory>

namespace CHTLJS {

    /**
     * @class Generator
     * @brief Generates JavaScript code from a CHTL JS Abstract Syntax Tree (AST).
     */
    class Generator {
    public:
        /**
         * @brief Constructs a Generator with a SaltBridge for communication.
         * @param bridge The SaltBridge instance.
         */
        Generator(std::shared_ptr<CHTL::SaltBridge> bridge);

        /**
         * @brief Generates JavaScript code from the given AST.
         * @param program The root of the AST.
         * @return The generated JavaScript code as a string.
         */
        std::string Generate(const ProgramNode& program);

    private:
        void visit(const AstNode& node);
        void visit(const EnhancedSelectorNode& node);
        void visit(const RawJSNode& node);
        void visit(const ScriptLoaderNode& node);
        void visit(const ListenNode& node);
        void visit(const DelegateNode& node);
        void visit(const AnimateNode& node);
        void visit(const VirNode& node);

        std::shared_ptr<CHTL::SaltBridge> bridge_;
        std::string output_;
    };

}
