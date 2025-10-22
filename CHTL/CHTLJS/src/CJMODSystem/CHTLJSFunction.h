#ifndef CHTLJSFUNCTION_H
#define CHTLJSFUNCTION_H

#include <string>
#include <map>

class CHTLJSFunction {
public:
    static void CreateCHTLJSFunction(const std::string& definition);
    static void bindVirtualObject(const std::string& function_name);
    static bool supportsVirtualObject(const std::string& function_name);

private:
    static std::map<std::string, std::string> definitions;
    static std::map<std::string, bool> virtual_bindings;
};

#endif // CHTLJSFUNCTION_H
