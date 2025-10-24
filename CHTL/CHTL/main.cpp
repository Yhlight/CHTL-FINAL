#include <iostream>
#include "CHTLLexer/Lexer.h"
#include "CHTLParser/Parser.h"
#include "CHTLGenerator/Generator.h"

int main() {
    std::string source = "html { body { h1 { text: \"Hello, CHTL!\"; } } }";

    // Lexer
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    // Parser
    Parser parser(tokens);
    std::unique_ptr<Node> ast = parser.parse();

    // Generator
    Generator generator;
    std::string html = generator.generate(*ast);

    std::cout << html << std::endl;

    return 0;
}
