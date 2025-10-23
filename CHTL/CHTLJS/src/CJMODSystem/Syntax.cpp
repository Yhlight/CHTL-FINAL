#include "Syntax.h"
#include "Arg.h"
#include <sstream>
#include <algorithm>
#include <vector>

Arg Syntax::analyze(const std::string& syntax) {
    Arg arg_list;
    std::stringstream ss(syntax);
    std::string item;
    while (ss >> item) {
        AtomArgType type = AtomArgType::LITERAL;
        if (item == "$") {
            type = AtomArgType::PLACEHOLDER;
        } else if (item == "$?") {
            type = AtomArgType::OPTIONAL;
        } else if (item == "$!") {
            type = AtomArgType::REQUIRED;
        } else if (item == "$_") {
            type = AtomArgType::UNORDERED;
        } else if (item == "...") {
            type = AtomArgType::VARIADIC;
        }
        arg_list.add(AtomArg(item, type));
    }
    return arg_list;
}

bool Syntax::isObject(const std::string& value) {
    std::string trimmed = value;
    trimmed.erase(std::remove_if(trimmed.begin(), trimmed.end(), isspace), trimmed.end());
    return !trimmed.empty() && trimmed.front() == '{' && trimmed.back() == '}';
}

bool Syntax::isFunction(const std::string& value) {
    return value.find("function") != std::string::npos;
}

bool Syntax::isArray(const std::string& value) {
    std::string trimmed = value;
    trimmed.erase(std::remove_if(trimmed.begin(), trimmed.end(), isspace), trimmed.end());
    return !trimmed.empty() && trimmed.front() == '[' && trimmed.back() == ']';
}

bool Syntax::isCHTLJSFunction(const std::string& value) {
    // A simple check for a pattern like "name { key: value }"
    size_t brace_pos = value.find('{');
    size_t colon_pos = value.find(':');
    return brace_pos != std::string::npos && colon_pos != std::string::npos && colon_pos > brace_pos;
}
