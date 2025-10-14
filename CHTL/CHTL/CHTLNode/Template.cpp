#include "Template.h"
#include "Style.h"

namespace CHTL {

    std::string TemplateDefinitionStatement::ToString() const {
        std::string out = "[Template] @Style " + Name->ToString() + " {";
        for (const auto& prop : Properties) {
            out += " " + prop->ToString();
        }
        out += " }";
        return out;
    }

} // namespace CHTL
