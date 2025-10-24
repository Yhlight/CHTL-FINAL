#pragma once

#include <string>
#include <vector>
#include <memory>

class Node {
public:
    virtual ~Node() = default;
};

class ElementNode : public Node {
public:
    std::string tagName;
    std::vector<std::unique_ptr<Node>> children;
};

class TextNode : public Node {
public:
    std::string text;
};
