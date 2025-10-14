#include "Value.h"

namespace CHTL
{
    Value Value::operator+(const Value& other) const
    {
        if (unit == other.unit || other.unit.empty())
        {
            return {num + other.num, unit};
        }
        if (unit.empty())
        {
            return {num + other.num, other.unit};
        }
        throw std::runtime_error("Unit mismatch: cannot add '" + unit + "' and '" + other.unit + "'.");
    }

    Value Value::operator-(const Value& other) const
    {
        if (unit == other.unit || other.unit.empty())
        {
            return {num - other.num, unit};
        }
        if (unit.empty())
        {
            // This case is tricky. 5 - 10px = -5px ? Let's support it.
            return {num - other.num, other.unit};
        }
        throw std::runtime_error("Unit mismatch: cannot subtract '" + other.unit + "' from '" + unit + "'.");
    }

    Value Value::operator*(const Value& other) const
    {
        if (unit.empty() || other.unit.empty())
        {
            return {num * other.num, unit.empty() ? other.unit : unit};
        }
        throw std::runtime_error("Invalid operation: cannot multiply unit '" + unit + "' by unit '" + other.unit + "'.");
    }

    Value Value::operator/(const Value& other) const
    {
        if (!other.unit.empty())
        {
            throw std::runtime_error("Invalid operation: cannot divide by a unit '" + other.unit + "'.");
        }
        if (other.num == 0)
        {
            throw std::runtime_error("Division by zero.");
        }
        return {num / other.num, unit};
    }
}
