#pragma once

#include "Node.h"
#include "Expression.h"
#include <vector>
#include <memory>
#include <utility>

namespace CHTL {

    class StyleProperty; // Forward declaration

    class TemplateDefinitionStatement : public Statement {
    public:
        TemplateDefinitionStatement(Token token, std::unique_ptr<Identifier> name)
            : token(std::move(token)), Name(std::move(name)) {}

        std::string ToString() const override;

        Token token; // The '[Template]' token
        std::unique_ptr<Identifier> Name;
        std::vector<std::unique_ptr<StyleProperty>> Properties;
    };

    class UseTemplateStatement : public Statement {
    public:
        UseTemplateStatement(Token token, std::unique_ptr<Identifier> name)
            : token(std::move(token)), Name(std::move(name)) {}

        std::string ToString() const override {
            return "@Style " + Name->ToString() + ";";
        }

        Token token; // The '@Style' token
        std::unique_ptr<Identifier> Name;
    };

} // namespace CHTL
