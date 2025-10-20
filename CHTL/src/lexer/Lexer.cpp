#include "lexer/Lexer.h"
#include <cctype>

namespace CHTL
{
    Lexer::Lexer(const std::string& input, const std::unordered_map<std::string, Token>& keywords)
        : m_input(input), m_keywords(keywords)
    {
        readChar(); // Initialize the first character
    }

    void Lexer::readChar()
    {
        if (m_readPosition >= m_input.length())
        {
            m_char = 0; // EOF
        }
        else
        {
            m_char = m_input[m_readPosition];
        }
        m_position = m_readPosition;
        m_readPosition++;

        if (m_char == '\n')
        {
            m_line++;
            m_column = 0;
        }
        else
        {
            m_column++;
        }
    }

    char Lexer::peekChar() const
    {
        if (m_readPosition >= m_input.length())
        {
            return 0; // EOF
        }
        return m_input[m_readPosition];
    }

    void Lexer::skipWhitespace()
    {
        while (isspace(m_char))
        {
            readChar();
        }
    }

    void Lexer::skipSingleLineComment()
    {
        while (m_char != '\n' && m_char != 0)
        {
            readChar();
        }
    }

    void Lexer::skipMultiLineComment()
    {
        while (true)
        {
            if (m_char == '*' && peekChar() == '/')
            {
                readChar(); // consume '*'
                readChar(); // consume '/'
                break;
            }
            if (m_char == 0)
            {
                break;
            }
            readChar();
        }
    }

    bool Lexer::isLetter(char ch)
    {
        return isalpha(ch) || ch == '_';
    }

    std::string Lexer::readIdentifier()
    {
        size_t startPosition = m_position;
        while (isLetter(m_char) || isdigit(m_char) || m_char == '-')
        {
            readChar();
        }
        return m_input.substr(startPosition, m_position - startPosition);
    }

    // This function will need to be adapted for fully dynamic keywords.
    // For now, we'll keep it simple and assume the dynamic keywords map
    // will contain the full string like "[Template]".
    Token Lexer::readBlockKeyword()
    {
        int startLine = m_line;
        int startColumn = m_column;

        readChar(); // Consume '['
        std::string identifier = readIdentifier();
        if (m_char != ']')
        {
            return {TokenType::ILLEGAL, "[" + identifier, startLine, startColumn};
        }
        readChar(); // Consume ']'

        std::string full_block = "[" + identifier + "]";
        auto it = m_keywords.find(full_block);
        if (it != m_keywords.end())
        {
            Token tok = it->second;
            tok.line = startLine;
            tok.column = startColumn;
            return tok;
        }

        return {TokenType::ILLEGAL, full_block, startLine, startColumn};
    }

    std::string Lexer::readString(char quote_type)
    {
        size_t startPosition = m_position + 1;
        do
        {
            readChar();
        } while (m_char != quote_type && m_char != 0);

        std::string result = m_input.substr(startPosition, m_position - startPosition);
        readChar(); // consume closing quote
        return result;
    }

    std::string Lexer::readNumber()
    {
        size_t startPosition = m_position;
        while (isdigit(m_char) || m_char == '.')
        {
            readChar();
        }
        return m_input.substr(startPosition, m_position - startPosition);
    }

    std::string Lexer::readComment()
    {
        size_t startPosition = m_position;
        while (m_char != '\n' && m_char != 0)
        {
            readChar();
        }
        return m_input.substr(startPosition, m_position - startPosition);
    }

    Token Lexer::NextToken()
    {
        Token tok;
        skipWhitespace();

        tok.line = m_line;
        tok.column = m_column;

        switch (m_char)
        {
            case '=':
            case ':':
                tok = {TokenType::COLON, std::string(1, m_char), tok.line, tok.column};
                break;
            case ';':
                tok = {TokenType::SEMICOLON, std::string(1, m_char), tok.line, tok.column};
                break;
            case '+':
                tok = {TokenType::PLUS, std::string(1, m_char), tok.line, tok.column};
                break;
            case '-':
                tok = {TokenType::MINUS, std::string(1, m_char), tok.line, tok.column};
                break;
            case '*':
                if (peekChar() == '*') {
                    readChar(); // consume first '*'
                    tok = {TokenType::POWER, "**", tok.line, tok.column};
                } else {
                    tok = {TokenType::ASTERISK, std::string(1, m_char), tok.line, tok.column};
                }
                break;
            case '%':
                tok = {TokenType::MODULO, std::string(1, m_char), tok.line, tok.column};
                break;
            case '>':
                tok = {TokenType::GT, std::string(1, m_char), tok.line, tok.column};
                break;
            case '<':
                tok = {TokenType::LT, std::string(1, m_char), tok.line, tok.column};
                break;
            case '?':
                tok = {TokenType::QUESTION, std::string(1, m_char), tok.line, tok.column};
                break;
            case ',':
                tok = {TokenType::COMMA, std::string(1, m_char), tok.line, tok.column};
                break;
            case '/':
                if (peekChar() == '/') {
                    readChar(); readChar();
                    skipSingleLineComment();
                    return NextToken();
                } else if (peekChar() == '*') {
                    readChar(); readChar();
                    skipMultiLineComment();
                    return NextToken();
                } else {
                    tok = {TokenType::SLASH, std::string(1, m_char), tok.line, tok.column};
                }
                break;
            case '(': tok = {TokenType::LPAREN, "(", tok.line, tok.column}; break;
            case ')': tok = {TokenType::RPAREN, ")", tok.line, tok.column}; break;
            case '{': tok = {TokenType::LBRACE, "{", tok.line, tok.column}; break;
            case '}': tok = {TokenType::RBRACE, "}", tok.line, tok.column}; break;
            case '[':
                if (isLetter(peekChar())) {
                    return readBlockKeyword();
                } else {
                    tok = {TokenType::LBRACKET, "[", tok.line, tok.column};
                }
                break;
            case ']': tok = {TokenType::RBRACKET, "]", tok.line, tok.column}; break;
            case '@': tok = {TokenType::AT, "@", tok.line, tok.column}; break;
            case '.': tok = {TokenType::DOT, ".", tok.line, tok.column}; break;
            case '&': tok = {TokenType::AMPERSAND, "&", tok.line, tok.column}; break;
            case '"':
            case '\'':
                tok.type = TokenType::STRING;
                tok.literal = readString(m_char);
                return tok;
            case '#':
                if (peekChar() == ' ') {
                    readChar(); readChar();
                    tok.type = TokenType::COMMENT;
                    tok.literal = readComment();
                    return tok;
                } else {
                    tok = {TokenType::ILLEGAL, "#", tok.line, tok.column};
                }
                break;
            case 0:
                tok.type = TokenType::END_OF_FILE;
                tok.literal = "";
                return tok;
            default:
                if (isdigit(m_char)) {
                    tok.type = TokenType::NUMBER;
                    tok.literal = readNumber();
                    return tok;
                } else if (isLetter(m_char)) {
                    tok.literal = readIdentifier();
                    auto it = m_keywords.find(tok.literal);
                    if (it != m_keywords.end()) {
                        tok.type = it->second.type;
                    } else {
                        tok.type = TokenType::IDENT;
                    }
                    return tok;
                } else {
                    tok = {TokenType::ILLEGAL, std::string(1, m_char), tok.line, tok.column};
                }
                break;
        }

        readChar();
        return tok;
    }

    std::string Lexer::readRawBlockContent()
    {
        size_t start_pos = m_position;
        size_t scan_pos = m_position;
        int brace_level = 1;

        if (m_char == '{') brace_level++;
        if (m_char == '}') brace_level--;
        scan_pos = m_readPosition;

        while (brace_level > 0 && scan_pos < m_input.length())
        {
            char current_char = m_input[scan_pos];
            if (current_char == '{') brace_level++;
            else if (current_char == '}') brace_level--;
            scan_pos++;
        }

        size_t end_pos = (brace_level > 0) ? m_input.length() : scan_pos - 1;
        std::string content = (end_pos > start_pos) ? m_input.substr(start_pos, end_pos - start_pos) : "";

        while(m_position < end_pos) {
            readChar();
        }

        return content;
    }

} // namespace CHTL
