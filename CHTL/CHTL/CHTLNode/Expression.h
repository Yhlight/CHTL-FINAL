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

    class NumberLiteral : public Expression {
    public:
        NumberLiteral(Token token, double value, std::string unit)
            : token(std::move(token)), Value(value), Unit(std::move(unit)) {}

        std::string ToString() const override { return token.literal; }

        Token token;
        double Value;
        std::string Unit;
    };

    class InfixExpression : public Expression {
    public:
        InfixExpression(Token token, std::unique_ptr<Expression> left, std::string op, std::unique_ptr<Expression> right)
            : token(std::move(token)), Left(std::move(left)), Operator(std::move(op)), Right(std::move(right)) {}

        std::string ToString() const override {
            std::string out = "(";
            out += Left->ToString();
            out += " " + Operator + " ";
            out += Right->ToString();
            out += ")";
            return out;
        }

        Token token; // The operator token, e.g. +
        std::unique_ptr<Expression> Left;
        std::string Operator;
        std::unique_ptr<Expression> Right;
    };

} // namespace CHTL
