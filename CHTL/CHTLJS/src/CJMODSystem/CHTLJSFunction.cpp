#include "CHTLJSFunction.h"
#include <sstream>

std::map<std::string, std::string> CHTLJSFunction::definitions;
std::map<std::string, bool> CHTLJSFunction::virtual_bindings;

void CHTLJSFunction::CreateCHTLJSFunction(const std::string& definition) {
    std::stringstream ss(definition);
    std::string function_name;
    ss >> function_name;
    definitions[function_name] = definition;
}

void CHTLJSFunction::bindVirtualObject(const std::string& function_name) {
    virtual_bindings[function_name] = true;
}

bool CHTLJSFunction::supportsVirtualObject(const std::string& function_name) {
    return virtual_bindings.count(function_name) > 0 && virtual_bindings[function_name];
}
