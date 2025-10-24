#include <iostream>
#include <fstream>
#include <sstream>
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/HTMLGenerator.h"

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << path << std::endl;
        exit(1);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[]) {
    std::string filename = "hello.chtl";
    if (argc > 1) {
        filename = argv[1];
    }

    std::string source = readFile(filename);

    try {
        CHTL::Lexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.scanTokens();

        CHTL::Parser parser(tokens);
        std::unique_ptr<CHTL::ASTNode> ast = parser.parse();

        CHTL::HTMLGenerator generator;
        std::string html = generator.generate(*ast);

        std::cout << html << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
