#include "CHTL/CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTL/CHTLGenerator/Generator.h"
#include <iostream>
#include <cassert>

void test_style_blocks() {
    std::string source = R"(
        div {
            style {
                color: "red";
                font-size: "16px";
            }
        }
    )";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    std::unique_ptr<Node> root = parser.parse();

    Generator generator;
    std::string html = generator.generate(*root);

    std::string expected_html = "<div style=\"color: red;font-size: 16px;\"></div>";
    assert(html == expected_html);

    std::cout << "test_style_blocks passed." << std::endl;
}

int main() {
    test_style_blocks();
    return 0;
}
