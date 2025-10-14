#pragma once

#include "../CHTLNode/Node.h"
#include "../CHTLNode/Statement.h"
#include <map>
#include <string>

namespace CHTL {

    using ElementMap = std::map<std::string, const ElementStatement*>;

    class DocumentMapBuilder {
    public:
        ElementMap Build(const Program* program);

    private:
        void Traverse(const Node* node);

        ElementMap elementMap;
    };

} // namespace CHTL
