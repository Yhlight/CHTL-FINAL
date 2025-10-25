#include <iostream>
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filepath>" << std::endl;
        return 1;
    }

    CHTLLexer lexer(argv[1]);
    CHTLParser parser(lexer);
    std::unique_ptr<ASTNode> ast = parser.parse();

    if (ast) {
        std::cout << ast->toString() << std::endl;
    } else {
        std::cerr << "Failed to parse CHTL file." << std::endl;
        return 1;
    }

    return 0;
}
