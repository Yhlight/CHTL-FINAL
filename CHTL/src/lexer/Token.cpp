#include "lexer/Token.h"

namespace CHTL
{
    // 将TokenType转换为字符串以便打印
    std::string TokenTypeToString(TokenType type)
    {
        switch (type)
        {
            case TokenType::ILLEGAL:             return "ILLEGAL";
            case TokenType::END_OF_FILE:         return "EOF";
            case TokenType::COMMENT:             return "COMMENT";
            case TokenType::IDENT:               return "IDENT";
            case TokenType::STRING:              return "STRING";
            case TokenType::NUMBER:              return "NUMBER";
            case TokenType::KEYWORD_TEXT:        return "KEYWORD_TEXT";
            case TokenType::KEYWORD_STYLE:       return "KEYWORD_STYLE";
            case TokenType::KEYWORD_SCRIPT:      return "KEYWORD_SCRIPT";
            case TokenType::KEYWORD_TEMPLATE:    return "KEYWORD_TEMPLATE";
            case TokenType::KEYWORD_CUSTOM:      return "KEYWORD_CUSTOM";
            case TokenType::KEYWORD_ORIGIN:      return "KEYWORD_ORIGIN";
            case TokenType::KEYWORD_IMPORT:      return "KEYWORD_IMPORT";
            case TokenType::KEYWORD_NAMESPACE:   return "KEYWORD_NAMESPACE";
            case TokenType::KEYWORD_CONFIGURATION: return "KEYWORD_CONFIGURATION";
            case TokenType::KEYWORD_INHERIT:     return "KEYWORD_INHERIT";
            case TokenType::KEYWORD_DELETE:      return "KEYWORD_DELETE";
            case TokenType::KEYWORD_INSERT:      return "KEYWORD_INSERT";
            case TokenType::KEYWORD_AFTER:       return "KEYWORD_AFTER";
            case TokenType::KEYWORD_BEFORE:      return "KEYWORD_BEFORE";
            case TokenType::KEYWORD_REPLACE:     return "KEYWORD_REPLACE";
            case TokenType::KEYWORD_AT:          return "KEYWORD_AT";
            case TokenType::KEYWORD_TOP:         return "KEYWORD_TOP";
            case TokenType::KEYWORD_BOTTOM:      return "KEYWORD_BOTTOM";
            case TokenType::KEYWORD_FROM:        return "KEYWORD_FROM";
            case TokenType::KEYWORD_AS:          return "KEYWORD_AS";
            case TokenType::KEYWORD_EXCEPT:      return "KEYWORD_EXCEPT";
            case TokenType::KEYWORD_USE:         return "KEYWORD_USE";
            case TokenType::KEYWORD_HTML5:       return "KEYWORD_HTML5";
            case TokenType::COLON:               return "COLON";
            case TokenType::SEMICOLON:           return "SEMICOLON";
            case TokenType::PLUS:                return "PLUS";
            case TokenType::MINUS:               return "MINUS";
            case TokenType::ASTERISK:            return "ASTERISK";
            case TokenType::SLASH:               return "SLASH";
            case TokenType::MODULO:              return "MODULO";
            case TokenType::POWER:               return "POWER";
            case TokenType::GT:                  return "GT";
            case TokenType::LT:                  return "LT";
            case TokenType::LPAREN:              return "LPAREN";
            case TokenType::RPAREN:              return "RPAREN";
            case TokenType::LBRACE:              return "LBRACE";
            case TokenType::RBRACE:              return "RBRACE";
            case TokenType::LBRACKET:            return "LBRACKET";
            case TokenType::RBRACKET:            return "RBRACKET";
            case TokenType::AT:                  return "AT";
            case TokenType::DOT:                 return "DOT";
            case TokenType::AMPERSAND:           return "AMPERSAND";
            case TokenType::QUESTION:            return "QUESTION";
            case TokenType::COMMA:               return "COMMA";
            default:                             return "UNKNOWN";
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
