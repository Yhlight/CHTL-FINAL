#include "StyleCollector.h"

void StyleCollector::collect(RootNode& root) {
    collectedRules.clear();
    currentElement = nullptr;
    root.accept(*this);
}

void StyleCollector::visit(RootNode& node) {
    for (auto& child : node.children) {
        child->accept(*this);
    }
}

void StyleCollector::visit(ElementNode& node) {
    ElementNode* previousElement = currentElement;
    currentElement = &node;
    if (node.style) {
        node.style->accept(*this);
    }
    for (auto& child : node.children) {
        child->accept(*this);
    }
    currentElement = previousElement;
}

void StyleCollector::visit(StyleNode& node) {
    for (auto& rule : node.rules) {
        collectedRules.push_back({rule.get(), currentElement});
    }
}