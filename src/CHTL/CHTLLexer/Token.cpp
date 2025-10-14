#include "Token.h"
#include <sstream>

namespace CHTL {

bool Token::isKeyword() const {
    return type_ >= TokenType::KEYWORD_TEXT && 
           type_ <= TokenType::KEYWORD_HTML5;
}

bool Token::isOperator() const {
    return (type_ >= TokenType::EQUAL && type_ <= TokenType::OR) ||
           type_ == TokenType::ARROW;
}

bool Token::isLiteral() const {
    return type_ == TokenType::STRING_LITERAL ||
           type_ == TokenType::UNQUOTED_LITERAL ||
           type_ == TokenType::NUMBER_LITERAL;
}

std::string Token::toString() const {
    std::ostringstream oss;
    oss << "[" << getTypeName(type_) << "] '" << lexeme_ << "' at "
        << location_.filename << ":" << location_.line << ":" << location_.column;
    return oss.str();
}

std::string Token::getTypeName(TokenType type) {
    switch (type) {
        case TokenType::END_OF_FILE: return "EOF";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case TokenType::NUMBER_LITERAL: return "NUMBER_LITERAL";
        
        case TokenType::KEYWORD_TEXT: return "KEYWORD_TEXT";
        case TokenType::KEYWORD_STYLE: return "KEYWORD_STYLE";
        case TokenType::KEYWORD_SCRIPT: return "KEYWORD_SCRIPT";
        
        case TokenType::KEYWORD_TEMPLATE: return "KEYWORD_TEMPLATE";
        case TokenType::KEYWORD_CUSTOM: return "KEYWORD_CUSTOM";
        case TokenType::KEYWORD_ORIGIN: return "KEYWORD_ORIGIN";
        case TokenType::KEYWORD_IMPORT: return "KEYWORD_IMPORT";
        case TokenType::KEYWORD_NAMESPACE: return "KEYWORD_NAMESPACE";
        case TokenType::KEYWORD_CONFIGURATION: return "KEYWORD_CONFIGURATION";
        
        case TokenType::KEYWORD_AT_STYLE: return "KEYWORD_AT_STYLE";
        case TokenType::KEYWORD_AT_ELEMENT: return "KEYWORD_AT_ELEMENT";
        case TokenType::KEYWORD_AT_VAR: return "KEYWORD_AT_VAR";
        case TokenType::KEYWORD_AT_HTML: return "KEYWORD_AT_HTML";
        case TokenType::KEYWORD_AT_JAVASCRIPT: return "KEYWORD_AT_JAVASCRIPT";
        case TokenType::KEYWORD_AT_CHTL: return "KEYWORD_AT_CHTL";
        case TokenType::KEYWORD_AT_CONFIG: return "KEYWORD_AT_CONFIG";
        case TokenType::KEYWORD_AT_CJMOD: return "KEYWORD_AT_CJMOD";
        
        case TokenType::KEYWORD_INHERIT: return "KEYWORD_INHERIT";
        case TokenType::KEYWORD_DELETE: return "KEYWORD_DELETE";
        case TokenType::KEYWORD_INSERT: return "KEYWORD_INSERT";
        case TokenType::KEYWORD_AFTER: return "KEYWORD_AFTER";
        case TokenType::KEYWORD_BEFORE: return "KEYWORD_BEFORE";
        case TokenType::KEYWORD_REPLACE: return "KEYWORD_REPLACE";
        case TokenType::KEYWORD_AT_TOP: return "KEYWORD_AT_TOP";
        case TokenType::KEYWORD_AT_BOTTOM: return "KEYWORD_AT_BOTTOM";
        case TokenType::KEYWORD_FROM: return "KEYWORD_FROM";
        case TokenType::KEYWORD_AS: return "KEYWORD_AS";
        case TokenType::KEYWORD_EXCEPT: return "KEYWORD_EXCEPT";
        case TokenType::KEYWORD_USE: return "KEYWORD_USE";
        case TokenType::KEYWORD_HTML5: return "KEYWORD_HTML5";
        
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
        case TokenType::HASH: return "HASH";
        case TokenType::AT: return "AT";
        case TokenType::AMPERSAND: return "AMPERSAND";
        
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::STAR: return "STAR";
        case TokenType::SLASH: return "SLASH";
        case TokenType::PERCENT: return "PERCENT";
        case TokenType::DOUBLE_STAR: return "DOUBLE_STAR";
        
        case TokenType::GREATER: return "GREATER";
        case TokenType::LESS: return "LESS";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenType::LESS_EQUAL: return "LESS_EQUAL";
        case TokenType::EQUAL_EQUAL: return "EQUAL_EQUAL";
        case TokenType::NOT_EQUAL: return "NOT_EQUAL";
        
        case TokenType::AND: return "AND";
        case TokenType::OR: return "OR";
        
        case TokenType::QUESTION: return "QUESTION";
        case TokenType::ARROW: return "ARROW";
        
        case TokenType::COMMENT_LINE: return "COMMENT_LINE";
        case TokenType::COMMENT_BLOCK: return "COMMENT_BLOCK";
        case TokenType::COMMENT_GENERATOR: return "COMMENT_GENERATOR";
        
        case TokenType::UNKNOWN: return "UNKNOWN";
        case TokenType::INVALID: return "INVALID";
        
        default: return "UNKNOWN";
    }
}

std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << token.toString();
    return os;
}

} // namespace CHTL
