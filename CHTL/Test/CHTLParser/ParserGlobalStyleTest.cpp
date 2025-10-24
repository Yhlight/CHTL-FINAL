#include "CHTL/CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTL/CHTLGenerator/Generator.h"
#include <iostream>
#include <cassert>

void test_global_styles() {
    std::string source = R"(
        html {
            head {}
            body {
                div {
                    style {
                        .box {
                            color: "red";
                        }
                    }
                }
            }
        }
    )";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    Document doc = parser.parse();

    Generator generator;
    std::string html = generator.generate(doc);

    std::string expected_html = "<html><head><style>.box {color: red;}</style></head><body><div class=\"box\"></div></body></html>";
    assert(html == expected_html);

    std::cout << "test_global_styles passed." << std::endl;
}

int main() {
    test_global_styles();
    return 0;
}
