#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../CHTLLexer/Token.h"

// Base class for all nodes in the AST
class Node {
public:
    virtual ~Node() = default;
    virtual std::string TokenLiteral() const = 0;
    virtual std::string String() const = 0;
};

// Base class for all statement nodes
class Statement : public Node {
public:
    virtual void statementNode() const {} // Marker method
};

// Base class for all expression nodes
class Expression : public Node {
public:
    virtual void expressionNode() const {} // Marker method
};

// The root node of the AST
class Program : public Node {
public:
    std::vector<std::unique_ptr<Statement>> statements;

    std::string TokenLiteral() const override {
        if (!statements.empty()) {
            return statements[0]->TokenLiteral();
        }
        return "";
    }

    std::string String() const override {
        std::string out;
        for (const auto& s : statements) {
            out += s->String();
        }
        return out;
    }
};

// Represents an identifier
class Identifier : public Expression {
public:
    Token token; // The TOKEN_IDENTIFIER token
    std::string value;

    Identifier(Token token, std::string value) : token(token), value(std::move(value)) {}

    std::string TokenLiteral() const override { return token.literal; }
    std::string String() const override { return value; }
};