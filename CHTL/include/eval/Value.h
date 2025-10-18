#pragma once

#include <string>
#include <stdexcept>

namespace CHTL
{
enum class ValueType { EMPTY, NUMBER, STRING, BOOL };

    struct Value
    {
        ValueType type = ValueType::NUMBER;
        double num = 0.0;
        std::string unit = "";
        std::string str = "";
        bool boolean = false;

        // Overload operators for convenience
        Value operator+(const Value& other) const;
        Value operator-(const Value& other) const;
        Value operator*(const Value& other) const;
        Value operator/(const Value& other) const;
        Value operator>(const Value& other) const;
        Value operator<(const Value& other) const;
    };
}
