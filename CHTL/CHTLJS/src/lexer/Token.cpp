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
            default:
                return "UNKNOWN";
        }
    }

} // namespace CHTLJS
