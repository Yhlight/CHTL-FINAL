#include "CHTLJS/include/lexer/Token.h"

namespace CHTLJS
{
    std::string TokenTypeToString(TokenType type)
    {
        switch (type)
        {
            case TokenType::RAW_JS:
                return "RAW_JS";
            case TokenType::END_OF_FILE:
                return "END_OF_FILE";
            case TokenType::LBRACE_BRACE:
                return "LBRACE_BRACE";
            case TokenType::RBRACE_BRACE:
                return "RBRACE_BRACE";
            case TokenType::LBRACE:
                return "LBRACE";
            case TokenType::RBRACE:
                return "RBRACE";
            case TokenType::LPAREN:
                return "LPAREN";
            case TokenType::RPAREN:
                return "RPAREN";
            case TokenType::LBRACKET:
                return "LBRACKET";
            case TokenType::RBRACKET:
                return "RBRACKET";
            case TokenType::COMMA:
                return "COMMA";
            case TokenType::COLON:
                return "COLON";
            case TokenType::SEMICOLON:
                return "SEMICOLON";
            case TokenType::ASSIGN:
                return "ASSIGN";
            case TokenType::IDENT:
                return "IDENT";
            case TokenType::ARROW:
                return "ARROW";
            case TokenType::BIND_OP:
                return "BIND_OP";
            default:
                return "UNKNOWN";
        }
    }

} // namespace CHTLJS
