#include <iostream>
#include <fstream>
#include <sstream>
#include "CHTL/CHTLLexer/Lexer.h"

void printUsage(const char* programName) {
    std::cout << "用法: " << programName << " <输入文件.chtl>\n";
    std::cout << "选项:\n";
    std::cout << "  --tokens    仅输出 token 列表\n";
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

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    try {
        std::string filename = argv[1];
        bool tokensOnly = false;
        
        // 检查选项
        if (argc > 2 && std::string(argv[2]) == "--tokens") {
            tokensOnly = true;
        }
        
        // 读取文件
        std::string source = readFile(filename);
        
        // 词法分析
        CHTL::Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        if (tokensOnly) {
            // 仅输出 tokens
            for (const auto& token : tokens) {
                std::cout << token << "\n";
            }
        } else {
            std::cout << "CHTL 编译器 v0.1.0\n";
            std::cout << "输入文件: " << filename << "\n";
            std::cout << "Token 数量: " << tokens.size() << "\n\n";
            
            // TODO: 语法分析和代码生成
            std::cout << "注意: 完整的编译功能正在开发中...\n";
        }
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << "\n";
        return 1;
    }
}
