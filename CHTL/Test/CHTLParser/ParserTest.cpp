#include "CHTL/CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTL/CHTLParser/Parser.h"
#include <iostream>
#include <cassert>

void test_simple_parsing() {
    std::string source = "div { span { } }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    Document doc = parser.parse();
    std::unique_ptr<Node> root = std::move(doc.root);

    assert(root != nullptr);
    ElementNode* divNode = dynamic_cast<ElementNode*>(root.get());
    assert(divNode != nullptr);
    assert(divNode->tagName == "div");
    assert(divNode->children.size() == 1);

    ElementNode* spanNode = dynamic_cast<ElementNode*>(divNode->children[0].get());
    assert(spanNode != nullptr);
    assert(spanNode->tagName == "span");
    assert(spanNode->children.empty());

    std::cout << "test_simple_parsing passed." << std::endl;
}

int main() {
    test_simple_parsing();
    return 0;
}
