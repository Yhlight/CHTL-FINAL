#pragma once

#include "../../CHTL/CHTLParser/Expression.h"
#include <string>
#include <map>
#include <memory>

struct StyleRule {
    std::string selector;
    std::map<std::string, std::unique_ptr<Expression>> properties;
};
