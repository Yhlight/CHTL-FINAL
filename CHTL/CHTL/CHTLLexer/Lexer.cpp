#include "Lexer.h"
#include <cctype>
#include <unordered_map>

namespace CHTL {

    namespace {
        const std::unordered_map<std::string, TokenType> keywords = {
            {"text", TokenType::TEXT},
            {"style", TokenType::STYLE},
            {"script", TokenType::SCRIPT},
            {"if", TokenType::IF},
            {"else", TokenType::ELSE},
            {"template", TokenType::TEMPLATE},
            {"element", TokenType::ELEMENT},
            {"var", TokenType::VAR},
            {"inherit", TokenType::INHERIT},
            {"custom", TokenType::CUSTOM},
            {"delete", TokenType::DELETE},
            {"insert", TokenType::INSERT},
            {"after", TokenType::AFTER},
            {"before", TokenType::BEFORE},
            {"replace", TokenType::REPLACE},
            {"attop", TokenType::AT_TOP},
            {"atbottom", TokenType::AT_BOTTOM},
            {"origin", TokenType::ORIGIN},
            {"html", TokenType::HTML},
            {"javascript", TokenType::JAVASCRIPT},
            {"chtl", TokenType::CHTL},
            {"as", TokenType::AS},
            {"from", TokenType::FROM},
            {"namespace", TokenType::NAMESPACE},
            {"except", TokenType::EXCEPT},
            {"configuration", TokenType::CONFIGURATION},
            {"config", TokenType::CONFIG},
            {"use", TokenType::USE},
            {"html5", TokenType::HTML5},
        };
    }

    Lexer::Lexer(const std::string& input)
        : input(input), position(0), readPosition(0), ch(0) {
        ReadChar();
    }

    void Lexer::ReadChar() {
        if (readPosition >= input.length()) {
            ch = 0;
        }
        else {
            ch = input[readPosition];
        }
        position = readPosition;
        readPosition += 1;
    }

    char Lexer::PeekChar() const {
        if (readPosition >= input.length()) {
            return 0;
        }
        else {
            return input[readPosition];
        }
    }

    void Lexer::SkipWhitespace() {
        while (isspace(ch)) {
            ReadChar();
        }
    }

    Token Lexer::NextToken() {
        Token tok;

        SkipWhitespace();

        switch (ch) {
        case '=':
             if (PeekChar() == '=') {
                ReadChar();
                tok = { TokenType::EQ, "==" };
            }
            else {
                tok = { TokenType::ASSIGN, "=" };
            }
            break;
        case '+':
            tok = { TokenType::PLUS, "+" };
            break;
        case '-':
            tok = { TokenType::MINUS, "-" };
            break;
        case '!':
            if (PeekChar() == '=') {
                ReadChar();
                tok = { TokenType::NOT_EQ, "!=" };
            }
            else {
                tok = { TokenType::BANG, "!" };
            }
            break;
        case '/':
            if (PeekChar() == '/') {
                while (ch != '\n' && ch != 0) {
                    ReadChar();
                }
                return NextToken(); // Recursively call NextToken to get the next token
            }
            else {
                tok = { TokenType::SLASH, "/" };
            }
            break;
        case '*':
            if (PeekChar() == '*') {
                ReadChar();
                tok = { TokenType::POWER, "**" };
            }
            else {
                tok = { TokenType::ASTERISK, "*" };
            }
            break;
        case '%':
            tok = {TokenType::MODULO, "%"};
            break;
        case '<':
            tok = { TokenType::LT, "<" };
            break;
        case '>':
            tok = { TokenType::GT, ">" };
            break;
        case ';':
            tok = { TokenType::SEMICOLON, ";" };
            break;
        case ':':
            tok = { TokenType::COLON, ":" };
            break;
        case ',':
            tok = { TokenType::COMMA, "," };
            break;
        case '(':
            tok = { TokenType::LPAREN, "(" };
            break;
        case ')':
            tok = { TokenType::RPAREN, ")" };
            break;
        case '{':
            tok = { TokenType::LBRACE, "{" };
            break;
        case '}':
            tok = { TokenType::RBRACE, "}" };
            break;
        case '[':
            tok = { TokenType::LBRACKET, "[" };
            break;
        case ']':
            tok = { TokenType::RBRACKET, "]" };
            break;
        case '"':
            tok.type = TokenType::STRING;
            tok.literal = ReadString();
            break;
        case 0:
            tok.literal = "";
            tok.type = TokenType::END_OF_FILE;
            break;
        default:
            if (isalpha(ch) || ch == '_') {
                tok.literal = ReadIdentifier();
                tok.type = LookupIdent(tok.literal);
                return tok;
            }
            else if (isdigit(ch)) {
                tok.type = TokenType::NUMBER;
                tok.literal = ReadNumber();
                return tok;
            }
            else {
                tok = { TokenType::ILLEGAL, std::string(1, ch) };
            }
        }

        ReadChar();
        return tok;
    }

    std::string Lexer::ReadIdentifier() {
        size_t startPosition = position;
        while (isalpha(ch) || ch == '_' || isdigit(ch) || ch == '-') {
            ReadChar();
        }
        return input.substr(startPosition, position - startPosition);
    }

    std::string Lexer::ReadNumber() {
        size_t startPosition = position;
        while (isdigit(ch)) {
            ReadChar();
        }
        return input.substr(startPosition, position - startPosition);
    }

    std::string Lexer::ReadString() {
        ReadChar(); // Consume the opening quote
        size_t startPosition = position;
        while (ch != '"' && ch != 0) {
            ReadChar();
        }
        return input.substr(startPosition, position - startPosition);
    }

    TokenType Lexer::LookupIdent(const std::string& ident) {
        if (keywords.count(ident)) {
            return keywords.at(ident);
        }
        return TokenType::IDENT;
    }

} // namespace CHTL
