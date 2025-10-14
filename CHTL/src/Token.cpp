#include "Token.h"

namespace CHTL
{
    std::unordered_map<std::string, TokenType> keywords = {
        {"text", TokenType::KEYWORD_TEXT},
        {"style", TokenType::KEYWORD_STYLE},
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
            case TokenType::KEYWORD_TEXT: return "TEXT";
            case TokenType::KEYWORD_STYLE:return "STYLE";
            case TokenType::ASSIGN:       return "=";
            case TokenType::COLON:        return ":";
            case TokenType::SEMICOLON:    return ";";
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
