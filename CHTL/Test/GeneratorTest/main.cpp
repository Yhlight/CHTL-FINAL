#include <iostream>
#include "../../CHTL/CHTLLexer/Lexer.h"
#include "../../CHTL/CHTLParser/Parser.h"
#include "../../CHTL/CHTLGenerator/Generator.h"

int main() {
    std::string input = R"(
        div {
            style {
                .class-a {
                    color: red;
                }
                .class-b {
                    background-color: blue;
                }
            }
        }
    )";

    CHTL::Lexer l(input);
    CHTL::Parser p(l);
    CHTL::Generator g;

    auto program = p.ParseProgram();
    if (program) {
        std::string output = g.Generate(program.get());
        std::cout << output << std::endl;
    } else {
        std::cout << "Parsing failed." << std::endl;
    }

    return 0;
}
