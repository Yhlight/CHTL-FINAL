#pragma once

#include "../CHTL/CHTLParser/AST.h"
#include "../CHTL/CSS/StyleRule.h"
#include <vector>
#include <memory>

struct Document {
    std::unique_ptr<Node> root;
    std::vector<StyleRule> globalStyles;
};
