#include <iostream>
#include <string>
#include "CHTL/CHTLLexer/Token.h"

int main(int argc, char* argv[]) {
    std::cout << "CHTL编译器 v0.1.0" << std::endl;
    std::cout << "基于C++20构建的超文本语言编译器" << std::endl;
    std::cout << std::endl;
    
    if (argc < 2) {
        std::cout << "用法: chtl <文件名.chtl>" << std::endl;
        std::cout << "当前版本为开发版本，仅包含基础Token功能" << std::endl;
        return 0;
    }
    
    std::string filename = argv[1];
    std::cout << "准备编译文件: " << filename << std::endl;
    
    // 演示Token功能
    CHTL::SourceLocation loc(filename, 1, 1);
    CHTL::Token token(CHTL::TokenType::IDENTIFIER, "div", loc);
    
    std::cout << "\n演示Token创建:" << std::endl;
    std::cout << token.toString() << std::endl;
    
    std::cout << "\n注意: 当前版本仅实现了Token基础结构" << std::endl;
    std::cout << "完整的词法分析器和语法分析器正在开发中..." << std::endl;
    
    return 0;
}
