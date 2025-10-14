#include <iostream>
#include <fstream>
#include <sstream>
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"

void printUsage(const char* programName) {
    std::cout << "╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                                ║\n";
    std::cout << "║             CHTL 编译器 - v0.4.0                               ║\n";
    std::cout << "║             将 CHTL 源代码编译为 HTML                          ║\n";
    std::cout << "║                                                                ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "📖 用法\n";
    std::cout << "════════════════════════════════════════════════════════════════\n";
    std::cout << "  " << programName << " <input.chtl> [选项]\n\n";
    
    std::cout << "⚙️  选项\n";
    std::cout << "════════════════════════════════════════════════════════════════\n";
    std::cout << "  --output <file>     输出文件路径（默认：标准输出）\n";
    std::cout << "  --doctype           添加 DOCTYPE 声明\n";
    std::cout << "  --compact           紧凑输出（无格式化）\n";
    std::cout << "  --tokens            显示词法分析结果（调试用）\n";
    std::cout << "  --ast               显示抽象语法树（调试用）\n";
    std::cout << "  --help, -h          显示此帮助信息\n\n";
    
    std::cout << "📝 示例\n";
    std::cout << "════════════════════════════════════════════════════════════════\n";
    std::cout << "  基础编译:\n";
    std::cout << "    " << programName << " input.chtl\n\n";
    std::cout << "  输出到文件:\n";
    std::cout << "    " << programName << " input.chtl --output output.html\n\n";
    std::cout << "  添加 DOCTYPE:\n";
    std::cout << "    " << programName << " input.chtl --doctype --output page.html\n\n";
    std::cout << "  紧凑输出:\n";
    std::cout << "    " << programName << " input.chtl --compact\n\n";
    
    std::cout << "✨ 功能特性\n";
    std::cout << "════════════════════════════════════════════════════════════════\n";
    std::cout << "  ✓ 内联样式 (style {} 块)\n";
    std::cout << "  ✓ 属性运算 (100px + 50px)\n";
    std::cout << "  ✓ 运算符优先级和括号\n";
    std::cout << "  ✓ CE 对等式 (: 和 = 等价)\n";
    std::cout << "  ✓ 完整的错误处理\n\n";
    
    std::cout << "📚 文档\n";
    std::cout << "════════════════════════════════════════════════════════════════\n";
    std::cout << "  README.md          - 项目概览\n";
    std::cout << "  TUTORIAL.md        - 快速教程\n";
    std::cout << "  EXAMPLES.md        - 示例集合\n";
    std::cout << "  CHTL.md            - 完整语法规范\n\n";
    
    std::cout << "💡 快速开始\n";
    std::cout << "════════════════════════════════════════════════════════════════\n";
    std::cout << "  查看示例: " << programName << " examples/simple.chtl\n";
    std::cout << "  阅读教程: cat TUTORIAL.md\n";
    std::cout << "  运行测试: python3 build.py --test\n\n";
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
    // 检查 --help 或 -h 标志
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        }
    }
    
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
