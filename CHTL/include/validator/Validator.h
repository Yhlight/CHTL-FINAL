#pragma once

#include "AstNode.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

namespace CHTL
{
    class Validator
    {
    public:
        Validator() = default;

        void Validate(const ProgramNode* program);
        const std::vector<std::string>& GetErrors() const { return m_errors; }

    private:
        void validateNode(const AstNode* node);
        void enterScope(const AstNode* node);
        void exitScope();
        bool isViolating(const AstNode* node);

        struct Scope
        {
            std::vector<ExceptNode::Constraint> constraints;
        };

        std::vector<Scope> m_scope_stack;
        std::vector<std::string> m_errors;
    };
}
