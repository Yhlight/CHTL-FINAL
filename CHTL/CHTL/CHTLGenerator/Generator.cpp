#include "Generator.h"

std::string Generator::generate(const Node& node) {
    std::string html;
    if (const ElementNode* element = dynamic_cast<const ElementNode*>(&node)) {
        html += "<" + element->tagName + ">";
        for (const auto& child : element->children) {
            html += generate(*child);
        }
        html += "</" + element->tagName + ">";
    } else if (const TextNode* text = dynamic_cast<const TextNode*>(&node)) {
        html += text->text;
    }
    return html;
}
