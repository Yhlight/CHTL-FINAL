#ifndef CHTLJSFUNCTION_H
#define CHTLJSFUNCTION_H

#include <string>
#include <map>

/**
 * @class CHTLJSFunction
 * @brief Manages the creation and properties of CHTL JS functions.
 *
 * This class provides a way to define new CHTL JS functions and associate them
 * with features like virtual object (`Vir`) support.
 */
class CHTLJSFunction {
public:
    /**
     * @brief Creates a new CHTL JS function from a definition string.
     * @param definition The string that defines the function's syntax.
     */
    static void CreateCHTLJSFunction(const std::string& definition);

    /**
     * @brief Binds virtual object support to a CHTL JS function.
     * @param function_name The name of the function to bind.
     */
    static void bindVirtualObject(const std::string& function_name);

    /**
     * @brief Checks if a given CHTL JS function supports virtual objects.
     * @param function_name The name of the function to check.
     * @return True if the function supports virtual objects, false otherwise.
     */
    static bool supportsVirtualObject(const std::string& function_name);

private:
    static std::map<std::string, std::string> definitions;
    static std::map<std::string, bool> virtual_bindings;
};

#endif // CHTLJSFUNCTION_H
