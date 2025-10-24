#pragma once

#include <string>
#include <map>

struct StyleRule {
    std::string selector;
    std::map<std::string, std::string> properties;
};
