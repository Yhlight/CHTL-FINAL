#pragma once

#include <cstddef>
#include <sstream>

namespace CHTL {

// 编译器配置和限制
struct CompilerConfig {
    // 输入限制
    static constexpr size_t MAX_FILE_SIZE = 100 * 1024 * 1024;  // 100MB
    static constexpr size_t MAX_LINE_LENGTH = 10000;             // 最大行长度
    static constexpr size_t MAX_TOKEN_COUNT = 1000000;           // 最大 Token 数
    
    // Parser 限制
    static constexpr size_t MAX_NESTING_DEPTH = 500;             // 最大嵌套深度
    static constexpr size_t MAX_ATTRIBUTES = 1000;               // 最大属性数
    static constexpr size_t MAX_CHILDREN = 10000;                // 最大子节点数
    
    // 输出限制
    static constexpr size_t MAX_OUTPUT_SIZE = 500 * 1024 * 1024; // 500MB
    
    // 性能配置
    static constexpr size_t VECTOR_RESERVE_SIZE = 100;           // 预留容器大小
    static constexpr size_t STRING_RESERVE_SIZE = 1024;          // 预留字符串大小
    
    // 超时配置（毫秒）
    static constexpr size_t COMPILE_TIMEOUT = 30000;             // 30秒编译超时
    
    // 调试配置
    bool verbose = false;                                        // 详细输出
    bool showWarnings = true;                                    // 显示警告
    bool showStats = false;                                      // 显示统计信息
};

// 编译统计信息
struct CompilerStats {
    size_t tokenCount = 0;          // Token 数量
    size_t nodeCount = 0;           // AST 节点数量
    size_t lineCount = 0;           // 行数
    size_t outputSize = 0;          // 输出大小
    double lexerTime = 0.0;         // 词法分析时间（秒）
    double parserTime = 0.0;        // 语法分析时间（秒）
    double generatorTime = 0.0;     // 代码生成时间（秒）
    double totalTime = 0.0;         // 总时间（秒）
    size_t warningCount = 0;        // 警告数量
    size_t errorCount = 0;          // 错误数量
    
    void reset() {
        *this = CompilerStats{};
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "编译统计:\n"
            << "  Token 数量: " << tokenCount << "\n"
            << "  AST 节点数: " << nodeCount << "\n"
            << "  源代码行数: " << lineCount << "\n"
            << "  输出大小: " << outputSize << " 字节\n"
            << "  词法分析: " << (lexerTime * 1000) << " ms\n"
            << "  语法分析: " << (parserTime * 1000) << " ms\n"
            << "  代码生成: " << (generatorTime * 1000) << " ms\n"
            << "  总耗时: " << (totalTime * 1000) << " ms\n"
            << "  警告: " << warningCount << " 个\n"
            << "  错误: " << errorCount << " 个";
        return oss.str();
    }
};

} // namespace CHTL
