#pragma once

#include "Node.h"
#include "Expression.h"
#include "Statement.h"
#include <vector>
#include <memory>
#include <utility>

namespace CHTL {

    class StyleProperty; // Forward declaration
    class BlockStatement; // Forward declaration

    class TemplateDefinitionStatement : public Statement {
    public:
        TemplateDefinitionStatement(Token token, std::unique_ptr<Identifier> name)
            : token(std::move(token)), Name(std::move(name)) {}

        std::string ToString() const override;

        Token token; // The '[Template]' token
        std::unique_ptr<Identifier> Name;
        std::unique_ptr<BlockStatement> Body;
        std::vector<std::unique_ptr<StyleProperty>> Properties;
    };

    class UseTemplateStatement : public Statement {
    public:
        UseTemplateStatement(Token token, std::unique_ptr<Identifier> name)
            : token(std::move(token)), Name(std::move(name)) {}

        std::string ToString() const override {
            if (token.type == TokenType::AT_STYLE) {
                return "@Style " + Name->ToString() + ";";
            }
            return "@Element " + Name->ToString() + ";";
        }

        Token token; // The '@Style' or '@Element' token
        std::unique_ptr<Identifier> Name;
    };

} // namespace CHTL
