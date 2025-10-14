#pragma once

#include "AstNode.h"
#include <string>
#include <sstream>

namespace CHTL
{
    class Generator
    {
    public:
        std::string Generate(ProgramNode* program);

    private:
        void visit(AstNode* node);
        void visit(ProgramNode* node);
        void visit(ElementNode* node);
        void visit(TextNode* node);
        // StyleNode is handled inside ElementNode's visit method

        std::stringstream m_output;
    };
}
