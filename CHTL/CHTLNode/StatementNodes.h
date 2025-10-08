#pragma once

#include "Node.h"
#include <vector>
#include <memory>

class ExpressionStatement : public Statement {
public:
    Token token; // the first token of the expression
    std::unique_ptr<Expression> expression;

    std::string TokenLiteral() const override { return token.literal; }
    std::string String() const override {
        if (expression) {
            return expression->String();
        }
        return "";
    }
};

class BlockStatement : public Statement {
public:
    Token token; // the { token
    std::vector<std::unique_ptr<Statement>> statements;

    std::string TokenLiteral() const override { return token.literal; }
    std::string String() const override {
        std::string out;
        for (const auto& s : statements) {
            out += s->String();
        }
        return out;
    }
};

class ElementStatement : public Statement {
public:
    Token token; // The element tag token (e.g., 'div')
    std::unique_ptr<Identifier> tagName;
    std::unique_ptr<BlockStatement> body;

    std::string TokenLiteral() const override { return token.literal; }
    std::string String() const override {
        std::string out;
        out += TokenLiteral() + " ";
        if (body) {
            out += body->String();
        }
        return out;
    }
};

class AttributeStatement : public Statement {
public:
    Token token; // The attribute name token
    std::unique_ptr<Identifier> name;
    std::unique_ptr<Expression> value;

    std::string TokenLiteral() const override { return token.literal; }
    std::string String() const override {
        std::string out;
        out += name->String() + ": ";
        if (value) {
            out += value->String();
        }
        out += ";";
        return out;
    }
};

class TextStatement : public Statement {
public:
    Token token; // The 'text' keyword token
    std::unique_ptr<BlockStatement> body;

    std::string TokenLiteral() const override { return token.literal; }
    std::string String() const override {
        std::string out;
        out += "text ";
        if (body) {
            out += body->String();
        }
        return out;
    }
};