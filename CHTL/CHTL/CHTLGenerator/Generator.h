#pragma once

#include "../CHTLParser/AST.h"
#include <string>

class Generator {
public:
    std::string generate(const Node& node);
};
