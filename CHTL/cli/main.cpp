#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include "loader/Loader.h"
#include "parser/Parser.h"
#include "generator/Generator.h"
#include "CMOD/packer.h"

void show_usage() {
    std::cout << "Usage: chtl <command> [options]" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  compile <file>   Compile a CHTL file" << std::endl;
    std::cout << "  package <input> <output>  Package a CHTL file into a .cmod file" << std::endl;
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

            auto bridge = std::make_shared<CHTL::ConcreteSaltBridge>();
            parser.SetBridge(bridge);
            CHTL::Generator generator(bridge);
            std::string html = generator.Generate(program.get());
            std::cout << html << std::endl;

        } catch (const std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    } else if (command == "package" && args.size() > 1) {
        std::string input_dir = args[1];
        std::string output_file = (args.size() > 2) ? args[2] : std::filesystem::path(input_dir).filename().string() + ".cmod";

        try {
            std::filesystem::path src_path = std::filesystem::path(input_dir) / "src";
            std::filesystem::path info_path = std::filesystem::path(input_dir) / "info";

            if (!std::filesystem::is_directory(src_path) || !std::filesystem::is_directory(info_path)) {
                throw std::runtime_error("Input directory must contain 'src' and 'info' subdirectories.");
            }

            auto merged_program = std::make_unique<CHTL::ProgramNode>();

            for (const auto& entry : std::filesystem::recursive_directory_iterator(src_path)) {
                if (entry.is_regular_file() && entry.path().extension() == ".chtl") {
                    std::string content = CHTL::Loader::ReadFile("", entry.path().string());
                    CHTL::Lexer lexer(content);
                    CHTL::Parser parser(lexer, entry.path().string());
                    auto program_part = parser.ParseProgram();

                    if (!parser.GetErrors().empty()) {
                        for (const auto& error : parser.GetErrors()) {
                            std::cerr << "Parser Error in " << entry.path().string() << ": " << error << std::endl;
                        }
                        return 1; // Stop on first error
                    }

                    // Merge logic
                    for (auto& child : program_part->children) {
                        merged_program->children.push_back(std::move(child));
                    }
                    for (auto const& [ns, def_map] : program_part->templates) {
                        for (auto const& [name, def_ptr] : def_map) {
                            merged_program->templates[ns][name] = def_ptr;
                        }
                    }
                    for (auto const& [ns, def_map] : program_part->customs) {
                        for (auto const& [name, def_ptr] : def_map) {
                            merged_program->customs[ns][name] = def_ptr;
                        }
                    }
                }
            }

            CHTL::CMOD::Packer::Pack(*merged_program, output_file);
            std::cout << "Successfully packaged module " << input_dir << " to " << output_file << std::endl;

        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    } else {
        show_usage();
        return 1;
    }

    return 0;
}
