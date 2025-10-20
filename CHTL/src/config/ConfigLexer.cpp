#include "config/ConfigLexer.h"
#include <vector>

namespace CHTL
{
    ConfigLexer::ConfigLexer(const std::string& input)
        : m_input(input)
    {
        readChar(); // Initialize m_char and m_readPosition
    }

    void ConfigLexer::readChar()
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
        m_readPosition += 1;
        m_column += 1;
    }

    char ConfigLexer::peekChar() const
    {
        if (m_readPosition >= m_input.length())
        {
            return 0;
        }
        return m_input[m_readPosition];
    }

    void ConfigLexer::skipWhitespace()
    {
        while (m_char == ' ' || m_char == '\t' || m_char == '\n' || m_char == '\r')
        {
            if (m_char == '\n')
            {
                m_line++;
                m_column = 0;
            }
            readChar();
        }
    }

    bool ConfigLexer::isLetter(char ch)
    {
        return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_';
    }

    std::string ConfigLexer::readIdentifier()
    {
        size_t startPosition = m_position;
        while (isLetter(m_char) || isdigit(m_char))
        {
            readChar();
        }
        return m_input.substr(startPosition, m_position - startPosition);
    }

    Token ConfigLexer::NextToken()
    {
        Token tok;
        skipWhitespace();

        tok.line = m_line;
        tok.column = m_column;

        switch (m_char)
        {
        case '[':
            tok = {TokenType::LBRACKET, "[", m_line, m_column};
            break;
        case ']':
            tok = {TokenType::RBRACKET, "]", m_line, m_column};
            break;
        case '@':
            tok = {TokenType::AT, "@", m_line, m_column};
            break;
        case ',':
            tok = {TokenType::COMMA, ",", m_line, m_column};
            break;
        case 0:
            tok = {TokenType::END_OF_FILE, "", m_line, m_column};
            break;
        default:
            if (isLetter(m_char))
            {
                tok.literal = readIdentifier();
                tok.type = TokenType::IDENT;
                // No need to call readChar() again, readIdentifier handles it.
                return tok;
            }
            else
            {
                tok = {TokenType::ILLEGAL, std::string(1, m_char), m_line, m_column};
            }
            break;
        }

        readChar();
        return tok;
    }

} // namespace CHTL
