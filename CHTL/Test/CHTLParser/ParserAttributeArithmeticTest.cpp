#include "CHTL/CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTL/CHTLGenerator/Generator.h"
#include <iostream>
#include <cassert>

void test_attribute_arithmetic() {
    std::string source = R"(
        div {
            style {
                width: 100px + 50px;
            }
        }
    )";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    Document doc = parser.parse();

    Generator generator;
    std::string html = generator.generate(doc);

    std::string expected_html = "<div style=\"width: 150.000000px;\"></div>";
    assert(html == expected_html);

    std::cout << "test_attribute_arithmetic passed." << std::endl;
}

int main() {
    test_attribute_arithmetic();
    return 0;
}
