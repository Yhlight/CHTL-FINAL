#include <iostream>
#include "../../CHTL/CHTLLexer/Lexer.h"

int main() {
    std::string input = R"(
        div {
            id = "box-1";
            class: welcome;
            text {
                "Hello, World!"
            }
        }
    )";

    CHTL::Lexer l(input);

    for (CHTL::Token tok = l.NextToken(); tok.type != CHTL::TokenType::END_OF_FILE; tok = l.NextToken()) {
        std::cout << "Type: " << static_cast<int>(tok.type) << ", Literal: " << tok.literal << std::endl;
    }

    return 0;
}
