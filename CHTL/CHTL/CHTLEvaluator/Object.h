#pragma once

#include <string>

namespace CHTL {

    enum class ObjectType {
        NUMBER,
        NULL_OBJ,
    };

    struct Object {
        ObjectType Type;
        double Value;
        std::string Unit;
    };

} // namespace CHTL
