#pragma once

#include <string>
#include <stdexcept>

namespace CHTL
{
    enum class ValueType { NUMBER, STRING };

    struct Value
    {
        ValueType type = ValueType::NUMBER;
        double num = 0.0;
        std::string unit = "";
        std::string str = "";

        // Overload operators for convenience
        Value operator+(const Value& other) const;
        Value operator-(const Value& other) const;
        Value operator*(const Value& other) const;
        Value operator/(const Value& other) const;
    };
}
