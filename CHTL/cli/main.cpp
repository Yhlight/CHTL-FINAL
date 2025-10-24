#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "loader/Loader.h"
#include "parser/Parser.h"
#include "generator/Generator.h"
#include "validator/Validator.h"
#include "formatter/Formatter.h"
#include "bridge/ConcreteSaltBridge.h"
#include "AstNode.h"

void show_usage() {
    std::cout << "Usage: chtl <command> [options]" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  compile <file>     Compile a CHTL file to HTML" << std::endl;
    std::cout << "  format <file>      Format a CHTL file" << std::endl;
    std::cout << "    --output <file>  Write formatted output to file" << std::endl;
    std::cout << "    --check          Check if file is formatted (exit code 1 if not)" << std::endl;
    std::cout << "    --indent <n>     Set indent size (default: 4)" << std::endl;
    std::cout << "    --tabs           Use tabs instead of spaces" << std::endl;
    std::cout << "  serialize <file>   Serialize a CHTL file to binary AST" << std::endl;
    std::cout << "  help               Show this help message" << std::endl;
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
    } else if (command == "format" && args.size() > 1) {
        std::string file_path = args[1];
        std::string output_file;
        bool check_only = false;
        CHTL::FormatterOptions options;
        
        // 解析格式化选项
        for (size_t i = 2; i < args.size(); ++i) {
            if (args[i] == "--output" && i + 1 < args.size()) {
                output_file = args[++i];
            } else if (args[i] == "--check") {
                check_only = true;
            } else if (args[i] == "--indent" && i + 1 < args.size()) {
                options.indentSize = std::stoi(args[++i]);
            } else if (args[i] == "--tabs") {
                options.useTabs = true;
            }
        }
        
        try {
            std::string content = CHTL::Loader::ReadFile("", file_path);
            CHTL::Formatter formatter(options);
            std::string formatted = formatter.Format(content);
            
            if (check_only) {
                // 检查模式：比较原文件和格式化后的内容
                if (content != formatted) {
                    std::cerr << "File is not formatted correctly" << std::endl;
                    return 1;
                } else {
                    std::cout << "File is correctly formatted" << std::endl;
                    return 0;
                }
            }
            
            if (!output_file.empty()) {
                // 写入输出文件
                std::ofstream ofs(output_file);
                if (!ofs) {
                    std::cerr << "Error: Cannot write to " << output_file << std::endl;
                    return 1;
                }
                ofs << formatted;
                std::cout << "Formatted output written to " << output_file << std::endl;
            } else {
                // 输出到标准输出
                std::cout << formatted;
            }
            
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
