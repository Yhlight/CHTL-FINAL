#include "CHTL/CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTL/CHTLGenerator/Generator.h"
#include <iostream>
#include <cassert>

void test_attributes_and_comments() {
    std::string source = R"(
        // This is a comment
        div {
            id = "main";
            class: "container"; /* multi-line
                                  comment */
            # generator comment
        }
    )";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    std::unique_ptr<Node> root = parser.parse();

    Generator generator;
    std::string html = generator.generate(*root);

    std::string expected_html = "<div class=\"container\" id=\"main\"></div>";
    assert(html == expected_html);

    std::cout << "test_attributes_and_comments passed." << std::endl;
}

int main() {
    test_attributes_and_comments();
    return 0;
}
