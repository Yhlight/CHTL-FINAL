#include "eval/Value.h"

namespace CHTL
{
    Value Value::operator+(const Value& other) const
    {
        Value val;
        val.type = ValueType::NUMBER;
        // If units are the same, or one of them is empty, the operation is valid.
        if (unit == other.unit || unit.empty() || other.unit.empty())
        {
            val.num = num + other.num;
            val.unit = unit.empty() ? other.unit : unit; // Use the non-empty unit.
        }
        else
        {
            throw std::runtime_error("Unit mismatch: cannot add '" + unit + "' and '" + other.unit + "'.");
        }

        if (!string_template.empty())
        {
            val.string_template = string_template;
        }
        else if (!other.string_template.empty())
        {
            val.string_template = other.string_template;
        }
        return val;
    }

    Value Value::operator-(const Value& other) const
    {
        Value val;
        val.type = ValueType::NUMBER;
        // If units are the same, or one of them is empty, the operation is valid.
        if (unit == other.unit || unit.empty() || other.unit.empty())
        {
            val.num = num - other.num;
            val.unit = unit.empty() ? other.unit : unit; // Use the non-empty unit.
        }
        else
        {
            throw std::runtime_error("Unit mismatch: cannot subtract '" + other.unit + "' from '" + unit + "'.");
        }

        if (!string_template.empty())
        {
            val.string_template = string_template;
        }
        else if (!other.string_template.empty())
        {
            val.string_template = other.string_template;
        }
        return val;
    }

    Value Value::operator*(const Value& other) const
    {
        Value val;
        val.type = ValueType::NUMBER;
        // Multiplication is valid if at least one operand has no unit.
        if (unit.empty() || other.unit.empty())
        {
            val.num = num * other.num;
            val.unit = unit.empty() ? other.unit : unit; // Use the non-empty unit.
        }
        else
        {
            // Multiplying two units is not allowed (e.g., 10px * 2px).
            throw std::runtime_error("Invalid operation: cannot multiply unit '" + unit + "' by unit '" + other.unit + "'.");
        }

        if (!string_template.empty())
        {
            val.string_template = string_template;
        }
        else if (!other.string_template.empty())
        {
            val.string_template = other.string_template;
        }
        return val;
    }

    Value Value::operator/(const Value& other) const
    {
        if (other.num == 0)
        {
            throw std::runtime_error("Division by zero.");
        }
        Value val;
        val.type = ValueType::NUMBER;
        val.num = num / other.num;

        if (unit == other.unit)
        {
            val.unit = ""; // Units cancel out
        }
        else if (other.unit.empty())
        {
            val.unit = unit; // Keep original unit
        }
        else
        {
             throw std::runtime_error("Unit mismatch: cannot divide '" + unit + "' by '" + other.unit + "'.");
        }

        if (!string_template.empty())
        {
            val.string_template = string_template;
        }
        else if (!other.string_template.empty())
        {
            val.string_template = other.string_template;
        }
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
