#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"

using namespace CHTL;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    Lexer lexer(source);
    Parser parser(lexer);
    auto ast = parser.ParseProgram();

    if (!parser.Errors().empty()) {
        std::cerr << "Parser errors:" << std::endl;
        for (const auto& msg : parser.Errors()) {
            std::cerr << "\t" << msg << std::endl;
        }
        return 1;
    }

    Generator generator;
    std::string html = generator.Generate(ast.get());

    std::cout << html << std::endl;

    return 0;
}
