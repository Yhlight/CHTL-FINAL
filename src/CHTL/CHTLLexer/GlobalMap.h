#ifndef CHTL_GLOBALMAP_H
#define CHTL_GLOBALMAP_H

#include <string>
#include <unordered_map>
#include "Token.h"

namespace CHTL {

/**
 * 关键字映射表
 * 负责将字符串映射到对应的TokenType
 */
class GlobalMap {
public:
    GlobalMap();
    
    /**
     * 查找关键字对应的TokenType
     * @param lexeme 词素
     * @return 如果是关键字，返回对应的TokenType；否则返回TokenType::IDENTIFIER
     */
    TokenType lookup(const std::string& lexeme) const;
    
    /**
     * 判断是否为关键字
     */
    bool isKeyword(const std::string& lexeme) const;
    
    /**
     * 判断是否为HTML元素标签
     */
    bool isHTMLElement(const std::string& lexeme) const;
    
private:
    void initializeKeywords();
    void initializeHTMLElements();
    
    std::unordered_map<std::string, TokenType> keywords_;
    std::unordered_map<std::string, bool> htmlElements_;
};

} // namespace CHTL

#endif // CHTL_GLOBALMAP_H
