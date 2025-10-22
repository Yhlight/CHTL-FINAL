#include "eval/Value.h"

namespace CHTL
{
    Value Value::operator+(const Value& other) const
    {
        Value val;
        val.type = ValueType::NUMBER;
        if (unit == other.unit || other.unit.empty())
        {
            val.num = num + other.num;
            val.unit = unit;
            return val;
        }
        if (unit.empty())
        {
            val.num = num + other.num;
            val.unit = other.unit;
            return val;
        }
        throw std::runtime_error("Unit mismatch: cannot add '" + unit + "' and '" + other.unit + "'.");
    }

    Value Value::operator-(const Value& other) const
    {
        Value val;
        val.type = ValueType::NUMBER;
        if (unit == other.unit || other.unit.empty())
        {
            val.num = num - other.num;
            val.unit = unit;
            return val;
        }
        if (unit.empty())
        {
            val.num = num - other.num;
            val.unit = other.unit;
            return val;
        }
        throw std::runtime_error("Unit mismatch: cannot subtract '" + other.unit + "' from '" + unit + "'.");
    }

    Value Value::operator*(const Value& other) const
    {
        Value val;
        val.type = ValueType::NUMBER;
        if (unit.empty() || other.unit.empty())
        {
            val.num = num * other.num;
            val.unit = unit.empty() ? other.unit : unit;
            return val;
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
        Value val;
        val.type = ValueType::NUMBER;
        val.num = num / other.num;
        val.unit = unit;
        return val;
    }

    Value Value::operator>(const Value& other) const
    {
        if (type != ValueType::NUMBER || other.type != ValueType::NUMBER)
        {
            throw std::runtime_error("Comparison operators only apply to numbers.");
        }
        Value val;
        val.type = ValueType::BOOL;
        val.boolean = num > other.num;
        return val;
    }

    Value Value::operator<(const Value& other) const
    {
        if (type != ValueType::NUMBER || other.type != ValueType::NUMBER)
        {
            throw std::runtime_error("Comparison operators only apply to numbers.");
        }
        Value val;
        val.type = ValueType::BOOL;
        val.boolean = num < other.num;
        return val;
    }
}
