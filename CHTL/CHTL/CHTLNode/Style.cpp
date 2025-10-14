#include "Style.h"
#include "Template.h"

namespace CHTL {

    std::string StyleStatement::ToString() const {
        std::string out = "style {";
        for (const auto& use : Uses) {
            out += " " + use->ToString();
        }
        for (const auto& prop : Properties) {
            out += " " + prop->ToString();
        }
        for (const auto& rule : Rules) {
            out += " " + rule->ToString();
        }
        out += " }";
        return out;
    }

} // namespace CHTL
