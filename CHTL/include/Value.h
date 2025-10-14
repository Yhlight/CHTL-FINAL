#pragma once

#include <string>
#include <stdexcept>

namespace CHTL
{
    struct Value
    {
        double num = 0.0;
        std::string unit = "";

        // Overload operators for convenience
        Value operator+(const Value& other) const;
        Value operator-(const Value& other) const;
        Value operator*(const Value& other) const;
        Value operator/(const Value& other) const;
    };
}
