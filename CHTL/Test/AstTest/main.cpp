#include <iostream>
#include "../../CHTL/CHTLLexer/Lexer.h"
#include "../../CHTL/CHTLParser/Parser.h"

void PrintParserErrors(const std::vector<std::string>& errors) {
    std::cerr << "Parser has " << errors.size() << " errors" << std::endl;
    for (const std::string& msg : errors) {
        std::cerr << "parser error: " << msg << std::endl;
    }
}

int main() {
    std::string input = R"(
        div {
            style {
                width: 100 + 50 * 2;
            }
        }
    )";

    CHTL::Lexer l(input);
    CHTL::Parser p(l);

    auto program = p.ParseProgram();
    if (!p.Errors().empty()) {
        PrintParserErrors(p.Errors());
        return 1;
    }

    if (program) {
        std::cout << program->ToString() << std::endl;
    } else {
        std::cout << "Parsing failed but no errors were reported." << std::endl;
    }

    return 0;
}
