#ifndef SYNTAX_H
#define SYNTAX_H

#include <string>
#include <vector>

// Forward declaration of Arg class
class Arg;

/**
 * @class Syntax
 * @brief Provides a set of static utility functions for analyzing and validating CHTL JS syntax.
 */
class Syntax {
public:
    /**
     * @brief Analyzes a syntax string and breaks it down into a list of arguments.
     * @param syntax The syntax string to analyze (e.g., "$ ** $").
     * @return An Arg object containing the parsed arguments.
     */
    static Arg analyze(const std::string& syntax);

    /**
     * @brief Checks if a given string represents a JavaScript object.
     * @param value The string to check.
     * @return True if the string is an object, false otherwise.
     */
    static bool isObject(const std::string& value);

    /**
     * @brief Checks if a given string represents a JavaScript function.
     * @param value The string to check.
     * @return True if the string is a function, false otherwise.
     */
    static bool isFunction(const std::string& value);

    /**
     * @brief Checks if a given string represents a JavaScript array.
     * @param value The string to check.
     * @return True if the string is an array, false otherwise.
     */
    static bool isArray(const std::string& value);

    /**
     * @brief Checks if a given string represents a CHTL JS function call.
     * @param value The string to check.
     * @return True if the string is a CHTL JS function, false otherwise.
     */
    static bool isCHTLJSFunction(const std::string& value);
};

#endif // SYNTAX_H
