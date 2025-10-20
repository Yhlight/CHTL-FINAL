#pragma once

#include <string>

namespace CHTLJS
{
    // 定义 CHTL JS 的 Token 类型
    enum class TokenType
    {
        // 特殊 Token
        RAW_JS,       // 原生 JS 内容
        END_OF_FILE,  // 文件结束符

        // 增强选择器
        LBRACE_BRACE,   // {{
        RBRACE_BRACE,   // }}

        // ... 其他 CHTL JS 的 token 将在这里添加
    };

    // Token 结构体
    struct Token
    {
        TokenType type;
        std::string literal; // Token 字面量
        int line = 0;        // Token 所在行号
        int column = 0;      // Token 所在列号
    };

    // 将 TokenType 转换为字符串以便打印（用于调试）
    std::string TokenTypeToString(TokenType type);

} // namespace CHTLJS
