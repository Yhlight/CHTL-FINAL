#include <iostream>
#include <fstream>
#include <sstream>
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"

void printUsage(const char* programName) {
    std::cout << "用法: " << programName << " <输入文件.chtl> [选项]\n\n";
    std::cout << "选项:\n";
    std::cout << "  --tokens              仅输出 token 列表\n";
    std::cout << "  --ast                 仅输出 AST\n";
    std::cout << "  --output <文件>       指定输出文件（默认：标准输出）\n";
    std::cout << "  --doctype             在 HTML 输出中包含 DOCTYPE\n";
    std::cout << "  --compact             紧凑模式（不格式化）\n";
    std::cout << "  --help, -h            显示此帮助信息\n\n";
    std::cout << "示例:\n";
    std::cout << "  " << programName << " input.chtl\n";
    std::cout << "  " << programName << " input.chtl --output output.html\n";
    std::cout << "  " << programName << " input.chtl --doctype --compact\n";
}

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开文件: " + filename);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("无法写入文件: " + filename);
    }
    file << content;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    try {
        std::string inputFile = argv[1];
        std::string outputFile;
        bool tokensOnly = false;
        bool astOnly = false;
        bool showHelp = false;
        
        CHTL::GeneratorConfig config;
        config.prettyPrint = true;
        config.includeDoctype = false;
        
        // 解析命令行参数
        for (int i = 2; i < argc; i++) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                showHelp = true;
            } else if (arg == "--tokens") {
                tokensOnly = true;
            } else if (arg == "--ast") {
                astOnly = true;
            } else if (arg == "--doctype") {
                config.includeDoctype = true;
            } else if (arg == "--compact") {
                config.prettyPrint = false;
            } else if (arg == "--output" && i + 1 < argc) {
                outputFile = argv[++i];
            }
        }
        
        if (showHelp) {
            printUsage(argv[0]);
            return 0;
        }
        
        // 读取输入文件
        std::string source = readFile(inputFile);
        
        // 词法分析
        CHTL::Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        if (tokensOnly) {
            // 仅输出 tokens
            for (const auto& token : tokens) {
                std::cout << token << "\n";
            }
            return 0;
        }
        
        // 语法分析
        CHTL::Parser parser(std::move(tokens));
        auto ast = parser.parse();
        
        if (parser.hasErrors()) {
            std::cerr << "解析错误:\n";
            for (const auto& error : parser.getErrors()) {
                std::cerr << "  " << error << "\n";
            }
            return 1;
        }
        
        if (astOnly) {
            // 仅输出 AST（简单形式）
            std::cout << "AST 节点数量: " << ast.size() << "\n";
            return 0;
        }
        
        // 代码生成
        CHTL::Generator generator(config);
        std::string html = generator.generate(ast);
        
        // 输出结果
        if (outputFile.empty()) {
            // 输出到标准输出
            std::cout << html;
        } else {
            // 输出到文件
            writeFile(outputFile, html);
            std::cerr << "已生成: " << outputFile << "\n";
        }
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << "\n";
        return 1;
    }
}
