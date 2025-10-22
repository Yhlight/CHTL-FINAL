#include "CHTLJS/include/lexer/Lexer.h"
#include <cctype>

namespace CHTLJS
{

    Lexer::Lexer(const std::string& input)
        : m_input(input)
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

    Token Lexer::NextToken()
    {
        Token tok;

        tok.line = m_line;
        tok.column = m_column;

        if (m_char == '{' && peekChar() == '{')
        {
            readChar(); // consume first '{'
            readChar(); // consume second '{'
            tok = {TokenType::LBRACE_BRACE, "{{", tok.line, tok.column};
            return tok;
        }
        else if (m_char == '}' && peekChar() == '}')
        {
            readChar(); // consume first '}'
            readChar(); // consume second '}'
            tok = {TokenType::RBRACE_BRACE, "}}", tok.line, tok.column};
            return tok;
        }
        else if (m_char == 0)
        {
            tok = {TokenType::END_OF_FILE, "", tok.line, tok.column};
            return tok;
        }
        else
        {
            size_t start_pos = m_position;
            while (m_char != 0 && !(m_char == '{' && peekChar() == '{') && !(m_char == '}' && peekChar() == '}'))
            {
                readChar();
            }
            tok.type = TokenType::RAW_JS;
            tok.literal = m_input.substr(start_pos, m_position - start_pos);
            return tok;
        }
    }
}
