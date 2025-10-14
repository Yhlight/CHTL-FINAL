#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include <string>
#include <memory>
#include "Token.h"
#include "GlobalMap.h"

namespace CHTL {

/**
 * 词法分析器
 * 将源代码字符流转换为Token流
 */
class Lexer {
public:
    /**
     * 构造函数
     * @param source 源代码字符串
     * @param filename 文件名（用于错误报告）
     */
    Lexer(const std::string& source, const std::string& filename = "");
    
    /**
     * 获取下一个Token
     * @return 下一个Token
     */
    Token nextToken();
    
    /**
     * 预览下一个Token（不消耗）
     * @return 下一个Token
     */
    Token peekToken();
    
    /**
     * 获取当前位置信息
     */
    SourceLocation getCurrentLocation() const;
    
private:
    // 字符流操作
    char peek() const;
    char peekNext() const;
    char advance();
    bool isAtEnd() const;
    bool match(char expected);
    
    // 跳过空白和注释
    void skipWhitespace();
    void skipLineComment();
    void skipBlockComment();
    Token scanGeneratorComment();
    
    // Token扫描
    Token scanToken();
    Token scanIdentifierOrKeyword();
    Token scanNumber();
    Token scanString(char quote);
    
    // 辅助函数
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    
    // 源代码和位置信息
    std::string source_;
    std::string filename_;
    size_t current_;
    size_t line_;
    size_t column_;
    size_t lineStart_; // 当前行的起始位置
    
    // 全局映射表
    GlobalMap globalMap_;
    
    // 预览Token缓存
    bool hasPeeked_;
    Token peekedToken_;
};

} // namespace CHTL

#endif // CHTL_LEXER_H
