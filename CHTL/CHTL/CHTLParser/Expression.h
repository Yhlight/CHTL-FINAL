#pragma once

#include <string>
#include <memory>

enum class Operator {
    Add,
    Subtract,
    Multiply,
    Divide
};

class Expression {
public:
    virtual ~Expression() = default;
};

class StringLiteral : public Expression {
public:
    std::string value;
};

class NumberLiteral : public Expression {
public:
    double value;
    std::string unit;
};

class BinaryExpr : public Expression {
public:
    std::unique_ptr<Expression> left;
    Operator op;
    std::unique_ptr<Expression> right;
};
