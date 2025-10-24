#include "Generator.h"

std::string Generator::generate(const Node& node) {
    std::string html;
    if (const ElementNode* element = dynamic_cast<const ElementNode*>(&node)) {
        html += "<" + element->tagName;
        for (const auto& attr : element->attributes) {
            html += " " + attr.first + "=\"" + attr.second + "\"";
        }
        if (!element->styles.empty()) {
            html += " style=\"";
            for (const auto& style : element->styles) {
                html += style.first + ": " + style.second + ";";
            }
            html += "\"";
        }
        html += ">";
        for (const auto& child : element->children) {
            html += generate(*child);
        }
        html += "</" + element->tagName + ">";
    } else if (const TextNode* text = dynamic_cast<const TextNode*>(&node)) {
        html += text->text;
    }
    return html;
}
