#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>

class Node {
public:
    virtual ~Node() = default;
};

class ElementNode : public Node {
public:
    std::string tagName;
    std::map<std::string, std::string> attributes;
    std::vector<std::unique_ptr<Node>> children;
};

class TextNode : public Node {
public:
    std::string text;
};
