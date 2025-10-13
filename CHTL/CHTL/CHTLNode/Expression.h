#pragma once

#include "Node.h"
#include "../CHTLLexer/Token.h"
#include <utility>

namespace CHTL {

    class Identifier : public Expression {
    public:
        Identifier(Token token, std::string value) : token(std::move(token)), value(std::move(value)) {}

        std::string ToString() const override { return value; }

        Token token; // The Token::IDENT token
        std::string value;
    };

    class StringLiteral : public Expression {
    public:
        StringLiteral(Token token, std::string value) : token(std::move(token)), value(std::move(value)) {}

        std::string ToString() const override { return token.literal; }

        Token token; // The Token::STRING token
        std::string value;
    };

    class UnquotedLiteral : public Expression {
    public:
        UnquotedLiteral(Token token, std::string value) : token(std::move(token)), value(std::move(value)) {}

        std::string ToString() const override { return value; }

        Token token;
        std::string value;
    };

} // namespace CHTL
