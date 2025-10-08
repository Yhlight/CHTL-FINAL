#include "Lexer.h"
#include <cctype>
#include <unordered_map>

namespace CHTL {

const std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"text", TokenType::KEYWORD_TEXT},
    {"style", TokenType::KEYWORD_STYLE},
    {"script", TokenType::KEYWORD_SCRIPT},
    {"inherit", TokenType::KEYWORD_INHERIT},
    {"delete", TokenType::KEYWORD_DELETE},
    {"insert", TokenType::KEYWORD_INSERT},
    {"after", TokenType::KEYWORD_AFTER},
    {"before", TokenType::KEYWORD_BEFORE},
    {"replace", TokenType::KEYWORD_REPLACE},
    {"at", TokenType::KEYWORD_AT},
    {"top", TokenType::KEYWORD_TOP},
    {"bottom", TokenType::KEYWORD_BOTTOM},
    {"from", TokenType::KEYWORD_FROM},
    {"as", TokenType::KEYWORD_AS},
    {"except", TokenType::KEYWORD_EXCEPT},
    {"use", TokenType::KEYWORD_USE},
    {"html5", TokenType::KEYWORD_HTML5},
    // Block keywords are handled separately by readBracketedKeyword
};

Lexer::Lexer(std::string input)
    : m_input(std::move(input)), m_position(0), m_readPosition(0), m_char(0), m_line(1), m_column(0) {
    readChar();
}

void Lexer::readChar() {
    if (m_readPosition >= m_input.length()) {
        m_char = 0; // EOF
    } else {
        m_char = m_input[m_readPosition];
    }
    m_position = m_readPosition;
    m_readPosition++;
    m_column++;
    if (m_char == '\n') {
        m_line++;
        m_column = 0;
    }
}

char Lexer::peekChar() const {
    if (m_readPosition >= m_input.length()) {
        return 0;
    }
    return m_input[m_readPosition];
}

void Lexer::skipWhitespace() {
    while (m_char == ' ' || m_char == '\t' || m_char == '\n' || m_char == '\r') {
        readChar();
    }
}

void Lexer::skipSingleLineComment() {
    while (m_char != '\n' && m_char != 0) {
        readChar();
    }
}

void Lexer::skipMultiLineComment() {
    while (m_char != 0) {
        if (m_char == '*' && peekChar() == '/') {
            readChar(); // Consume '*'
            readChar(); // Consume '/'
            break;
        }
        readChar();
    }
}

Token Lexer::readIdentifier() {
    size_t startPos = m_position;
    int startCol = m_column;
    while (isalnum(m_char) || m_char == '_') {
        readChar();
    }
    std::string literal = m_input.substr(startPos, m_position - startPos);

    auto it = keywords.find(literal);
    if (it != keywords.end()) {
        return {it->second, literal, m_line, startCol};
    }

    return {TokenType::IDENTIFIER, literal, m_line, startCol};
}

Token Lexer::readStringLiteral(char quote_type) {
    size_t startPos = m_position + 1; // Skip the opening quote
    int startCol = m_column;
    readChar(); // Consume opening quote
    while (m_char != quote_type && m_char != 0) {
        // TODO: Handle escaped quotes
        readChar();
    }
    std::string literal = m_input.substr(startPos, m_position - startPos);
    readChar(); // Consume closing quote
    return {TokenType::STRING_LITERAL, literal, m_line, startCol};
}

Token Lexer::readNumericLiteral() {
    size_t startPos = m_position;
    int startCol = m_column;
    bool has_dot = false;
    while (isdigit(m_char) || (m_char == '.' && !has_dot)) {
        if (m_char == '.') has_dot = true;
        readChar();
    }
    std::string literal = m_input.substr(startPos, m_position - startPos);
    return {TokenType::NUMERIC_LITERAL, literal, m_line, startCol};
}

Token Lexer::readBracketedKeyword() {
    int startCol = m_column;
    readChar(); // consume '['
    size_t startPos = m_position;
    while(m_char != ']' && m_char != 0) {
        readChar();
    }
    std::string literal = m_input.substr(startPos, m_position - startPos);
    readChar(); // consume ']'

    if (literal == "Custom") return {TokenType::KEYWORD_CUSTOM, literal, m_line, startCol};
    if (literal == "Template") return {TokenType::KEYWORD_TEMPLATE, literal, m_line, startCol};
    if (literal == "Origin") return {TokenType::KEYWORD_ORIGIN, literal, m_line, startCol};
    if (literal == "Import") return {TokenType::KEYWORD_IMPORT, literal, m_line, startCol};
    if (literal == "Namespace") return {TokenType::KEYWORD_NAMESPACE, literal, m_line, startCol};
    if (literal == "Configuration") return {TokenType::KEYWORD_CONFIGURATION, literal, m_line, startCol};
    if (literal == "Info") return {TokenType::KEYWORD_INFO, literal, m_line, startCol};
    if (literal == "Export") return {TokenType::KEYWORD_EXPORT, literal, m_line, startCol};

    return {TokenType::ILLEGAL, literal, m_line, startCol};
}


Token Lexer::nextToken() {
    skipWhitespace();

    int startCol = m_column;
    Token tok{TokenType::ILLEGAL, std::string(1, m_char), m_line, startCol};

    switch (m_char) {
        case '=':
            if (peekChar() == '=') {
                readChar();
                tok = {TokenType::EQUAL_EQUAL, "==", m_line, startCol};
            } else {
                tok = {TokenType::EQUAL, "=", m_line, startCol};
            }
            break;
        case ':': tok = {TokenType::COLON, ":", m_line, startCol}; break;
        case ';': tok = {TokenType::SEMICOLON, ";", m_line, startCol}; break;
        case '(': tok = {TokenType::L_PAREN, "(", m_line, startCol}; break;
        case ')': tok = {TokenType::R_PAREN, ")", m_line, startCol}; break;
        case '{': tok = {TokenType::L_BRACE, "{", m_line, startCol}; break;
        case '}': tok = {TokenType::R_BRACE, "}", m_line, startCol}; break;
        case ',': tok = {TokenType::COMMA, ",", m_line, startCol}; break;
        case '+': tok = {TokenType::PLUS, "+", m_line, startCol}; break;
        case '-':
             if (peekChar() == '>') {
                readChar();
                tok = {TokenType::ARROW, "->", m_line, startCol};
            } else {
                tok = {TokenType::MINUS, "-", m_line, startCol};
            }
            break;
        case '*':
            if (peekChar() == '*') {
                readChar();
                tok = {TokenType::POWER, "**", m_line, startCol};
            } else {
                tok = {TokenType::ASTERISK, "*", m_line, startCol};
            }
            break;
        case '/':
            if (peekChar() == '/') {
                skipSingleLineComment();
                return nextToken(); // Tail call to get the next actual token
            } else if (peekChar() == '*') {
                skipMultiLineComment();
                return nextToken(); // Tail call
            } else {
                tok = {TokenType::SLASH, "/", m_line, startCol};
            }
            break;
        case '<':
            if (peekChar() == '=') {
                readChar();
                tok = {TokenType::LESS_EQUAL, "<=", m_line, startCol};
            } else {
                tok = {TokenType::LESS_THAN, "<", m_line, startCol};
            }
            break;
        case '>':
             if (peekChar() == '=') {
                readChar();
                tok = {TokenType::GREATER_EQUAL, ">=", m_line, startCol};
            } else {
                tok = {TokenType::GREATER_THAN, ">", m_line, startCol};
            }
            break;
        case '!':
             if (peekChar() == '=') {
                readChar();
                tok = {TokenType::NOT_EQUAL, "!=", m_line, startCol};
            }
            break;
        case '&':
            if (peekChar() == '&') {
                readChar();
                tok = {TokenType::LOGICAL_AND, "&&", m_line, startCol};
            } else if (peekChar() == '-' && m_input[m_readPosition + 1] == '>') {
                 readChar(); readChar();
                 tok = {TokenType::EVENT_BIND_OP, "&->", m_line, startCol};
            }
            else {
                tok = {TokenType::AMPERSAND, "&", m_line, startCol};
            }
            break;
        case '|':
             if (peekChar() == '|') {
                readChar();
                tok = {TokenType::LOGICAL_OR, "||", m_line, startCol};
            }
            break;
        case '"':
        case '\'':
            return readStringLiteral(m_char);
        case '[':
            return readBracketedKeyword();
        case ']': tok = {TokenType::R_SQ_BRACKET, "]", m_line, startCol}; break;
        case '#': tok = {TokenType::HASH, "#", m_line, startCol}; break;
        case '.': tok = {TokenType::DOT, ".", m_line, startCol}; break;
        case '@': tok = {TokenType::AT_SIGN, "@", m_line, startCol}; break;
        case '$': tok = {TokenType::DOLLAR, "$", m_line, startCol}; break;
        case '?': tok = {TokenType::QUESTION_MARK, "?", m_line, startCol}; break;
        case '%': tok = {TokenType::PERCENT, "%", m_line, startCol}; break;
        case 0:
            tok = {TokenType::END_OF_FILE, "", m_line, startCol};
            break;
        default:
            if (isalpha(m_char) || m_char == '_') {
                return readIdentifier();
            } else if (isdigit(m_char)) {
                return readNumericLiteral();
            }
            // Note: Unquoted literals are context-dependent and are tricky to handle
            // in a context-free lexer. This will likely need to be handled by the parser.
            // For now, we'll treat them as illegal.
            break;
    }

    readChar();
    return tok;
}

} // namespace CHTL