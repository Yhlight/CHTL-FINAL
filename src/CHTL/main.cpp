#include <iostream>
#include <string>
#include <vector>
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLGenerator/CHTLGenerator.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filepath> [--dump-ast]" << std::endl;
        return 1;
    }

    std::string filepath;
    bool dumpAst = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--dump-ast") {
            dumpAst = true;
        } else {
            filepath = arg;
        }
    }

    if (filepath.empty()) {
        std::cerr << "Usage: " << argv[0] << " <filepath> [--dump-ast]" << std::endl;
        return 1;
    }

    CHTLLexer lexer(filepath);
    CHTLParser parser(lexer);
    std::unique_ptr<ProgramNode> ast = parser.parse();

    if (ast) {
        if (dumpAst) {
            std::cout << ast->toString() << std::endl;
        } else {
            CHTLGenerator generator(std::move(ast));
            std::cout << generator.generate() << std::endl;
        }
    } else {
        std::cerr << "Failed to parse CHTL file." << std::endl;
        return 1;
    }

    return 0;
}
