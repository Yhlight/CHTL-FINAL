#include <iostream>
#include "CHTLLexer/CHTLLexer.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filepath>" << std::endl;
        return 1;
    }

    CHTLLexer lexer(argv[1]);
    Token token = lexer.getNextToken();
    while (token.type != TokenType::END_OF_FILE) {
        std::cout << "Token(" << Token::typeToString(token.type) << ", \"" << token.value << "\")" << std::endl;
        token = lexer.getNextToken();
    }

    return 0;
}
