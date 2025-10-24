#pragma once

#include "../../SharedCore/Document.h"
#include <string>

class Generator {
public:
    std::string generate(Document& doc);
private:
    std::string generateNode(const Node& node);
    std::string evaluateExpression(const Expression& expr);
};
