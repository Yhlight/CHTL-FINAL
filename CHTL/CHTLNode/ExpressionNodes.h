#pragma once

#include "Node.h"
#include <memory>

class StringLiteral : public Expression {
public:
    Token token;
    std::string value;

    std::string TokenLiteral() const override { return token.literal; }
    std::string String() const override { return token.literal; }
};

class PrefixExpression : public Expression {
public:
    Token token; // The prefix token, e.g. !
    std::string op;
    std::unique_ptr<Expression> right;

    std::string TokenLiteral() const override { return token.literal; }
    std::string String() const override {
        std::string out;
        out += "(";
        out += op;
        out += right->String();
        out += ")";
        return out;
    }
};

class InfixExpression : public Expression {
public:
    Token token; // The operator token, e.g. +
    std::unique_ptr<Expression> left;
    std::string op;
    std::unique_ptr<Expression> right;

    std::string TokenLiteral() const override { return token.literal; }
    std::string String() const override {
        std::string out;
        out += "(";
        out += left->String();
        out += " " + op + " ";
        out += right->String();
        out += ")";
        return out;
    }
};

class ConditionalExpression : public Expression {
public:
    Token token; // The ? token
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Expression> consequence;
    std::unique_ptr<Expression> alternative;

    std::string TokenLiteral() const override { return token.literal; }
    std::string String() const override {
        std::string out;
        out += "(";
        out += condition->String();
        out += " ? ";
        out += consequence->String();
        out += " : ";
        out += alternative->String();
        out += ")";
        return out;
    }
};