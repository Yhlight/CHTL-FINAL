#include "Token.h"

namespace CHTL {

const char* tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::UNKNOWN: return "UNKNOWN";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::KEYWORD_TEXT: return "KEYWORD_TEXT";
        case TokenType::KEYWORD_STYLE: return "KEYWORD_STYLE";
        case TokenType::KEYWORD_SCRIPT: return "KEYWORD_SCRIPT";
        
        // CHTL 特殊语法块关键字
        case TokenType::KEYWORD_TEMPLATE: return "KEYWORD_TEMPLATE";
        case TokenType::KEYWORD_CUSTOM: return "KEYWORD_CUSTOM";
        case TokenType::KEYWORD_IMPORT: return "KEYWORD_IMPORT";
        case TokenType::KEYWORD_ORIGIN: return "KEYWORD_ORIGIN";
        case TokenType::KEYWORD_NAMESPACE: return "KEYWORD_NAMESPACE";
        case TokenType::KEYWORD_CONFIGURATION: return "KEYWORD_CONFIGURATION";
        case TokenType::KEYWORD_INFO: return "KEYWORD_INFO";
        case TokenType::KEYWORD_EXPORT: return "KEYWORD_EXPORT";
        
        // @ 前缀关键字
        case TokenType::KEYWORD_AT_STYLE: return "KEYWORD_AT_STYLE";
        case TokenType::KEYWORD_AT_ELEMENT: return "KEYWORD_AT_ELEMENT";
        case TokenType::KEYWORD_AT_VAR: return "KEYWORD_AT_VAR";
        case TokenType::KEYWORD_AT_HTML: return "KEYWORD_AT_HTML";
        case TokenType::KEYWORD_AT_JAVASCRIPT: return "KEYWORD_AT_JAVASCRIPT";
        case TokenType::KEYWORD_AT_CHTL: return "KEYWORD_AT_CHTL";
        case TokenType::KEYWORD_AT_CONFIG: return "KEYWORD_AT_CONFIG";
        case TokenType::KEYWORD_AT_CJMOD: return "KEYWORD_AT_CJMOD";
        
        // 其他关键字
        case TokenType::KEYWORD_FROM: return "KEYWORD_FROM";
        case TokenType::KEYWORD_AS: return "KEYWORD_AS";
        case TokenType::KEYWORD_INHERIT: return "KEYWORD_INHERIT";
        case TokenType::KEYWORD_DELETE: return "KEYWORD_DELETE";
        case TokenType::KEYWORD_INSERT: return "KEYWORD_INSERT";
        case TokenType::KEYWORD_USE: return "KEYWORD_USE";
        case TokenType::KEYWORD_EXCEPT: return "KEYWORD_EXCEPT";
        case TokenType::KEYWORD_AFTER: return "KEYWORD_AFTER";
        case TokenType::KEYWORD_BEFORE: return "KEYWORD_BEFORE";
        case TokenType::KEYWORD_REPLACE: return "KEYWORD_REPLACE";
        
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COLON: return "COLON";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::EQUALS: return "EQUALS";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::STAR: return "STAR";
        case TokenType::SLASH: return "SLASH";
        case TokenType::PERCENT: return "PERCENT";
        case TokenType::POWER: return "POWER";
        case TokenType::COMMENT_LINE: return "COMMENT_LINE";
        case TokenType::COMMENT_BLOCK: return "COMMENT_BLOCK";
        case TokenType::COMMENT_GENERATOR: return "COMMENT_GENERATOR";
        default: return "UNKNOWN";
    }
}

std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << "Token{" << tokenTypeToString(token.type) 
       << ", \"" << token.value << "\""
       << ", line: " << token.line 
       << ", col: " << token.column << "}";
    return os;
}

} // namespace CHTL
