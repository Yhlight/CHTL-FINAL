#include "Lexer.h"
#include <cctype>
#include <unordered_map>

static std::unordered_map<std::string, TokenType> keywords = {
    {"text", TokenType::TOKEN_TEXT}
};

Lexer::Lexer(const std::string& input) : input(input), position(0), readPosition(0), ch(0) {
    readChar();
}

void Lexer::readChar() {
    if (readPosition >= input.length()) {
        ch = 0; // EOF
    } else {
        ch = input[readPosition];
    }
    position = readPosition;
    readPosition += 1;
}

char Lexer::peekChar() {
    if (readPosition >= input.length()) {
        return 0;
    }
    return input[readPosition];
}

void Lexer::skipWhitespace() {
    while (isspace(ch)) {
        readChar();
    }
}

std::string Lexer::readIdentifier() {
    int startPosition = position;
    while (isalpha(ch) || ch == '_' || ch == '-') {
        readChar();
    }
    return input.substr(startPosition, position - startPosition);
}

std::string Lexer::readString(char delimiter) {
    readChar(); // consume opening quote
    int startPosition = position;
    while (ch != delimiter && ch != 0) {
        readChar();
    }
    std::string result = input.substr(startPosition, position - startPosition);
    readChar(); // consume closing quote
    return result;
}

std::string Lexer::readNumber() {
    int startPosition = position;
    while (isdigit(ch)) {
        readChar();
    }
    return input.substr(startPosition, position - startPosition);
}

std::string Lexer::readSingleLineComment() {
    int startPosition = position;
    while (ch != '\n' && ch != 0) {
        readChar();
    }
    return input.substr(startPosition, position - startPosition);
}

std::string Lexer::readMultiLineComment() {
    int startPosition = position;
    while (ch != 0) {
        if (ch == '*' && peekChar() == '/') {
            readChar();
            readChar();
            break;
        }
        readChar();
    }
    return input.substr(startPosition, position - startPosition);
}


Token Lexer::NextToken() {
    Token tok;

    skipWhitespace();

    switch (ch) {
        case '{': tok = {TokenType::TOKEN_LBRACE, "{"}; break;
        case '}': tok = {TokenType::TOKEN_RBRACE, "}"}; break;
        case ';': tok = {TokenType::TOKEN_SEMICOLON, ";"}; break;
        case ':': tok = {TokenType::TOKEN_COLON, ":"}; break;
        case '(': tok = {TokenType::TOKEN_LPAREN, "("}; break;
        case ')': tok = {TokenType::TOKEN_RPAREN, ")"}; break;
        case '[': tok = {TokenType::TOKEN_LBRACKET, "["}; break;
        case ']': tok = {TokenType::TOKEN_RBRACKET, "]"}; break;
        case ',': tok = {TokenType::TOKEN_COMMA, ","}; break;
        case '.': tok = {TokenType::TOKEN_DOT, "."}; break;
        case '+': tok = {TokenType::TOKEN_PLUS, "+"}; break;
        case '-': tok = {TokenType::TOKEN_MINUS, "-"}; break;
        case '%': tok = {TokenType::TOKEN_PERCENT, "%"}; break;
        case '>': tok = {TokenType::TOKEN_GT, ">"}; break;
        case '<': tok = {TokenType::TOKEN_LT, "<"}; break;
        case '?': tok = {TokenType::TOKEN_QUESTION, "?"}; break;
        case '=':
            tok = {TokenType::TOKEN_EQUAL, "="};
            break;
        case '*':
            if (peekChar() == '*') {
                readChar();
                tok = {TokenType::TOKEN_DOUBLE_ASTERISK, "**"};
            } else {
                tok = {TokenType::TOKEN_ASTERISK, "*"};
            }
            break;
        case '/':
            if (peekChar() == '/') {
                tok.type = TokenType::TOKEN_COMMENT;
                tok.literal = readSingleLineComment();
                return tok;
            } else if (peekChar() == '*') {
                tok.type = TokenType::TOKEN_COMMENT;
                tok.literal = readMultiLineComment();
                return tok;
            } else {
                tok = {TokenType::TOKEN_SLASH, "/"};
            }
            break;
        case '#':
            tok.type = TokenType::TOKEN_COMMENT;
            tok.literal = readSingleLineComment();
            return tok;
        case '!':
             if (peekChar() == '=') {
                char old_ch = ch;
                readChar();
                tok = {TokenType::TOKEN_BANG, std::string(1, old_ch) + std::string(1, ch)};
            } else {
                tok = {TokenType::TOKEN_BANG, "!"};
            }
            break;
        case '&':
            if (peekChar() == '&') {
                readChar();
                tok = {TokenType::TOKEN_AND, "&&"};
            } else {
                tok = {TokenType::TOKEN_ILLEGAL, "&"};
            }
            break;
        case '|':
            if (peekChar() == '|') {
                readChar();
                tok = {TokenType::TOKEN_OR, "||"};
            } else {
                tok = {TokenType::TOKEN_ILLEGAL, "|"};
            }
            break;
        case '"':
            tok.type = TokenType::TOKEN_STRING;
            tok.literal = readString('"');
            return tok;
        case '\'':
            tok.type = TokenType::TOKEN_SINGLE_QUOTE_STRING;
            tok.literal = readString('\'');
            return tok;
        case 0:
            tok = {TokenType::TOKEN_EOF, ""};
            break;
        default:
            if (isalpha(ch) || ch == '_') {
                tok.literal = readIdentifier();
                if (keywords.count(tok.literal)) {
                    tok.type = keywords[tok.literal];
                } else {
                    tok.type = TokenType::TOKEN_IDENTIFIER;
                }
                return tok;
            } else if (isdigit(ch)) {
                tok.type = TokenType::TOKEN_IDENTIFIER; // Treat numbers as identifiers for now
                tok.literal = readNumber();
                return tok;
            }
            else {
                tok = {TokenType::TOKEN_ILLEGAL, std::string(1, ch)};
            }
            break;
    }

    readChar();
    return tok;
}