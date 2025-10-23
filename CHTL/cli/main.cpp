#include <iostream>
#include <vector>
#include <string>
#include "loader/Loader.h"
#include "parser/Parser.h"
#include "generator/Generator.h"
#include "validator/Validator.h"
#include "bridge/ConcreteSaltBridge.h"
#include "AstNode.h"

void show_usage() {
    std::cout << "Usage: chtl <command> [options]" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  compile <file>   Compile a CHTL file" << std::endl;
    std::cout << "  serialize <file> Serialize a CHTL file to binary AST" << std::endl;
    std::cout << "  help             Show this help message" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        show_usage();
        return 1;
    }

    std::vector<std::string> args(argv + 1, argv + argc);
    std::string command = args[0];

    if (command == "help") {
        show_usage();
    } else if (command == "compile" && args.size() > 1) {
        std::string file_path = args[1];
        try {
            std::string content = CHTL::Loader::ReadFile("", file_path);
            CHTL::Lexer lexer(content);
            CHTL::Parser parser(lexer, file_path);
            auto program = parser.ParseProgram();

            if (!parser.GetErrors().empty()) {
                for (const auto& error : parser.GetErrors()) {
                    std::cerr << "Parser Error: " << error << std::endl;
                }
                return 1;
            }

            CHTL::Validator validator;
            validator.Validate(program.get());

            if (!validator.GetErrors().empty()) {
                for (const auto& error : validator.GetErrors()) {
                    std::cerr << "Validator Error: " << error << std::endl;
                }
                return 1;
            }

            auto bridge = std::make_shared<CHTL::ConcreteSaltBridge>();
            parser.SetBridge(bridge);
            CHTL::Generator generator(bridge);
            std::string html = generator.Generate(program.get());
            std::cout << html << std::endl;

        } catch (const std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    } else if (command == "serialize" && args.size() > 1) {
        std::string file_path = args[1];
        try {
            std::string content = CHTL::Loader::ReadFile("", file_path);
            CHTL::Lexer lexer(content);
            CHTL::Parser parser(lexer, file_path);
            auto program = parser.ParseProgram();

            if (!parser.GetErrors().empty()) {
                for (const auto& error : parser.GetErrors()) {
                    std::cerr << "Parser Error: " << error << std::endl;
                }
                return 1;
            }

            CHTL::Validator validator;
            validator.Validate(program.get());

            if (!validator.GetErrors().empty()) {
                for (const auto& error : validator.GetErrors()) {
                    std::cerr << "Validator Error: " << error << std::endl;
                }
                return 1;
            }

            program->serialize(std::cout);

        } catch (const std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    } else {
        show_usage();
        return 1;
    }

    return 0;
}
