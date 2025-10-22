#ifndef SYNTAX_H
#define SYNTAX_H

#include <string>
#include <vector>

// Forward declaration of Arg class
class Arg;

class Syntax {
public:
    static Arg analyze(const std::string& syntax);
    static bool isObject(const std::string& value);
    static bool isFunction(const std::string& value);
    static bool isArray(const std::string& value);
    static bool isCHTLJSFunction(const std::string& value);
};

#endif // SYNTAX_H
