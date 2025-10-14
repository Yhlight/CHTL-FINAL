#pragma once

#include <string>

namespace CHTL {

    enum class ObjectType {
        NUMBER,
        BOOLEAN,
        STRING,
        NULL_OBJ,
    };

    struct Object {
        ObjectType Type;
        double Value;
        std::string Unit;
        bool BoolValue;
        std::string StringValue;

        bool IsTruthy() const {
            switch (Type) {
                case ObjectType::BOOLEAN:
                    return BoolValue;
                case ObjectType::NUMBER:
                    return Value != 0;
                default:
                    return false;
            }
        }
    };

} // namespace CHTL
