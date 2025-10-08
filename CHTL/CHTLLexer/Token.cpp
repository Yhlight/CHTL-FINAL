#include "Token.h"

namespace CHTL {

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::ILLEGAL: return "ILLEGAL";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::NUMERIC_LITERAL: return "NUMERIC_LITERAL";
        case TokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case TokenType::COLON: return "COLON";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::ASTERISK: return "ASTERISK";
        case TokenType::SLASH: return "SLASH";
        case TokenType::PERCENT: return "PERCENT";
        case TokenType::POWER: return "POWER";
        case TokenType::GREATER_THAN: return "GREATER_THAN";
        case TokenType::LESS_THAN: return "LESS_THAN";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenType::LESS_EQUAL: return "LESS_EQUAL";
        case TokenType::EQUAL_EQUAL: return "EQUAL_EQUAL";
        case TokenType::NOT_EQUAL: return "NOT_EQUAL";
        case TokenType::LOGICAL_AND: return "LOGICAL_AND";
        case TokenType::LOGICAL_OR: return "LOGICAL_OR";
        case TokenType::QUESTION_MARK: return "QUESTION_MARK";
        case TokenType::ARROW: return "ARROW";
        case TokenType::EVENT_BIND_OP: return "EVENT_BIND_OP";
        case TokenType::L_BRACE: return "L_BRACE";
        case TokenType::R_BRACE: return "R_BRACE";
        case TokenType::L_PAREN: return "L_PAREN";
        case TokenType::R_PAREN: return "R_PAREN";
        case TokenType::L_SQ_BRACKET: return "L_SQ_BRACKET";
        case TokenType::R_SQ_BRACKET: return "R_SQ_BRACKET";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::HASH: return "HASH";
        case TokenType::AMPERSAND: return "AMPERSAND";
        case TokenType::DOLLAR: return "DOLLAR";
        case TokenType::AT_SIGN: return "AT_SIGN";
        case TokenType::SINGLE_LINE_COMMENT: return "SINGLE_LINE_COMMENT";
        case TokenType::MULTI_LINE_COMMENT: return "MULTI_LINE_COMMENT";
        case TokenType::KEYWORD_TEXT: return "KEYWORD_TEXT";
        case TokenType::KEYWORD_STYLE: return "KEYWORD_STYLE";
        case TokenType::KEYWORD_SCRIPT: return "KEYWORD_SCRIPT";
        case TokenType::KEYWORD_INHERIT: return "KEYWORD_INHERIT";
        case TokenType::KEYWORD_DELETE: return "KEYWORD_DELETE";
        case TokenType::KEYWORD_INSERT: return "KEYWORD_INSERT";
        case TokenType::KEYWORD_AFTER: return "KEYWORD_AFTER";
        case TokenType::KEYWORD_BEFORE: return "KEYWORD_BEFORE";
        case TokenType::KEYWORD_REPLACE: return "KEYWORD_REPLACE";
        case TokenType::KEYWORD_AT: return "KEYWORD_AT";
        case TokenType::KEYWORD_TOP: return "KEYWORD_TOP";
        case TokenType::KEYWORD_BOTTOM: return "KEYWORD_BOTTOM";
        case TokenType::KEYWORD_FROM: return "KEYWORD_FROM";
        case TokenType::KEYWORD_AS: return "KEYWORD_AS";
        case TokenType::KEYWORD_EXCEPT: return "KEYWORD_EXCEPT";
        case TokenType::KEYWORD_USE: return "KEYWORD_USE";
        case TokenType::KEYWORD_HTML5: return "KEYWORD_HTML5";
        case TokenType::KEYWORD_CUSTOM: return "KEYWORD_CUSTOM";
        case TokenType::KEYWORD_TEMPLATE: return "KEYWORD_TEMPLATE";
        case TokenType::KEYWORD_ORIGIN: return "KEYWORD_ORIGIN";
        case TokenType::KEYWORD_IMPORT: return "KEYWORD_IMPORT";
        case TokenType::KEYWORD_NAMESPACE: return "KEYWORD_NAMESPACE";
        case TokenType::KEYWORD_CONFIGURATION: return "KEYWORD_CONFIGURATION";
        case TokenType::KEYWORD_INFO: return "KEYWORD_INFO";
        case TokenType::KEYWORD_EXPORT: return "KEYWORD_EXPORT";
        case TokenType::TYPE_STYLE: return "TYPE_STYLE";
        case TokenType::TYPE_ELEMENT: return "TYPE_ELEMENT";
        case TokenType::TYPE_VAR: return "TYPE_VAR";
        case TokenType::TYPE_HTML: return "TYPE_HTML";
        case TokenType::TYPE_JAVASCRIPT: return "TYPE_JAVASCRIPT";
        case TokenType::TYPE_CHTL: return "TYPE_CHTL";
        case TokenType::TYPE_CJMOD: return "TYPE_CJMOD";
        case TokenType::TYPE_CONFIG: return "TYPE_CONFIG";
        default: return "UNKNOWN";
    }
}

} // namespace CHTL