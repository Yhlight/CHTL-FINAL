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
        std::string string_template = "";

        bool IsTruthy() const {
            switch (type) {
                case ValueType::BOOL:
                    return boolean;
                case ValueType::NUMBER:
                    return num != 0;
                case ValueType::STRING:
                    return !str.empty();
                case ValueType::EMPTY:
                    return false;
            }
            return false;
        }

        // Overload operators for convenience
        Value operator+(const Value& other) const;
        Value operator-(const Value& other) const;
        Value operator*(const Value& other) const;
        Value operator/(const Value& other) const;
        Value operator>(const Value& other) const;
        Value operator<(const Value& other) const;
    };
}
