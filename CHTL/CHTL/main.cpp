#include <iostream>
#include "CHTLLexer/Lexer.h"
#include "CHTLParser/Parser.h"
#include "CHTLGenerator/Generator.h"

int main() {
    std::string source = "html { head {} body { h1 { class: \"title\"; style { color: \"blue\"; .title { font-size: \"24px\"; } } } } }";

    // Lexer
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    // Parser
    Parser parser(tokens);
    Document doc = parser.parse();

    // Generator
    Generator generator;
    std::string html = generator.generate(doc);

    std::cout << html << std::endl;

    return 0;
}
