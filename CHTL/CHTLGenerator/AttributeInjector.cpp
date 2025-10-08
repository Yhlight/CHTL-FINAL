#include "AttributeInjector.h"

void AttributeInjector::inject(RootNode& root) {
    root.accept(*this);
}

void AttributeInjector::visit(RootNode& node) {
    for (auto& child : node.children) {
        child->accept(*this);
    }
}

void AttributeInjector::visit(ElementNode& node) {
    // Inject attributes for the current node
    if (node.style) {
        std::set<std::string> classes;
        if (node.attributes.count("class")) {
            std::stringstream ss(node.attributes["class"]);
            std::string item;
            while (std::getline(ss, item, ' ')) {
                if (!item.empty()) {
                    classes.insert(item);
                }
            }
        }

        for (const auto& rule : node.style->rules) {
            if (rule->selector.empty() || rule->selector.find('&') != std::string::npos) continue;

            if (rule->selector[0] == '.') {
                size_t endPos = rule->selector.find_first_of(":[. ");
                classes.insert(rule->selector.substr(1, endPos - 1));
            } else if (rule->selector[0] == '#') {
                size_t endPos = rule->selector.find_first_of(":[. ");
                node.attributes["id"] = rule->selector.substr(1, endPos - 1);
            }
        }

        if (!classes.empty()) {
            std::string classString;
            for (const auto& cls : classes) {
                classString += cls + " ";
            }
            classString.pop_back();
            node.attributes["class"] = classString;
        }
    }

    // Continue traversal to children
    for (auto& child : node.children) {
        child->accept(*this);
    }
}