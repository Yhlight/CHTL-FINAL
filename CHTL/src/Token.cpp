#include "Token.h"

namespace CHTL
{
    std::unordered_map<std::string, TokenType> keywords = {
        {"text", TokenType::KEYWORD_TEXT},
        {"style", TokenType::KEYWORD_STYLE},
        {"delete", TokenType::KEYWORD_DELETE},
        {"from", TokenType::KEYWORD_FROM},
    };

    // 将TokenType转换为字符串以便打印
    std::string TokenTypeToString(TokenType type)
    {
        switch (type)
        {
            case TokenType::ILLEGAL:      return "ILLEGAL";
            case TokenType::END_OF_FILE:  return "EOF";
            case TokenType::IDENT:        return "IDENT";
            case TokenType::STRING:       return "STRING";
            case TokenType::NUMBER:          return "NUMBER";
            case TokenType::KEYWORD_TEXT:      return "TEXT";
            case TokenType::KEYWORD_STYLE:     return "STYLE";
            case TokenType::KEYWORD_TEMPLATE:  return "TEMPLATE";
            case TokenType::KEYWORD_CUSTOM:    return "CUSTOM";
            case TokenType::KEYWORD_DELETE:    return "DELETE";
            case TokenType::KEYWORD_IMPORT:    return "IMPORT";
            case TokenType::KEYWORD_FROM:      return "FROM";
            case TokenType::KEYWORD_NAMESPACE: return "NAMESPACE";
            case TokenType::ASSIGN:          return "=";
            case TokenType::COLON:           return ":";
            case TokenType::SEMICOLON:    return ";";
            case TokenType::PLUS:         return "+";
            case TokenType::MINUS:        return "-";
            case TokenType::ASTERISK:     return "*";
            case TokenType::SLASH:        return "/";
            case TokenType::LPAREN:       return "(";
            case TokenType::RPAREN:       return ")";
            case TokenType::LBRACE:       return "{";
            case TokenType::RBRACE:       return "}";
            default:                      return "UNKNOWN";
        }
    }

    std::string Token::ToString() const
    {
        return "Token[Type: " + TokenTypeToString(type) +
               ", Literal: '" + literal +
               "', Line: " + std::to_string(line) +
               ", Column: " + std::to_string(column) + "]";
    }

} // namespace CHTL
