#include "lexer/Lexer.h"
#include <cctype>
#include <utility> // For std::move

namespace CHTL
{

Lexer::Lexer(const std::string& input, std::map<std::string, Token> keywords)
    : m_input(input), m_keywords(std::move(keywords))
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
            readChar();
            readChar();
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

    // Block keywords are not customizable, so we can check them directly.
    if (identifier == "Template")
    {
        return {TokenType::KEYWORD_TEMPLATE, "[Template]", startLine, startColumn};
    }
    else if (identifier == "Custom")
    {
        return {TokenType::KEYWORD_CUSTOM, "[Custom]", startLine, startColumn};
    }
    else if (identifier == "Import")
    {
        return {TokenType::KEYWORD_IMPORT, "[Import]", startLine, startColumn};
    }
    else if (identifier == "Namespace")
    {
        return {TokenType::KEYWORD_NAMESPACE, "[Namespace]", startLine, startColumn};
    }
    else if (identifier == "Origin")
    {
        return {TokenType::KEYWORD_ORIGIN, "[Origin]", startLine, startColumn};
    }
    else if (identifier == "Configuration")
    {
        return {TokenType::KEYWORD_CONFIGURATION, "[Configuration]", startLine, startColumn};
    }
    else if (identifier == "Name")
    {
        return {TokenType::KEYWORD_NAME, "[Name]", startLine, startColumn};
    }

    return {TokenType::ILLEGAL, "[" + identifier + "]", startLine, startColumn};
}

std::string Lexer::readString(char quote_type)
{
    size_t startPosition = m_position + 1;
    do
    {
        readChar();
    } while (m_char != quote_type && m_char != 0);

    std::string result = m_input.substr(startPosition, m_position - startPosition);
    readChar();
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
                readChar();
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
            if (peekChar() == '/')
            {
                readChar();
                readChar();
                skipSingleLineComment();
                return NextToken();
            }
            else if (peekChar() == '*')
            {
                readChar();
                readChar();
                skipMultiLineComment();
                return NextToken();
            }
            else
            {
                tok = {TokenType::SLASH, std::string(1, m_char), tok.line, tok.column};
            }
            break;
        case '(':
            tok = {TokenType::LPAREN, std::string(1, m_char), tok.line, tok.column};
            break;
        case ')':
            tok = {TokenType::RPAREN, std::string(1, m_char), tok.line, tok.column};
            break;
        case '{':
            tok = {TokenType::LBRACE, std::string(1, m_char), tok.line, tok.column};
            break;
        case '}':
            tok = {TokenType::RBRACE, std::string(1, m_char), tok.line, tok.column};
            break;
        case '[':
            if (isLetter(peekChar()))
            {
                return readBlockKeyword();
            }
            else
            {
                tok = {TokenType::LBRACKET, std::string(1, m_char), tok.line, tok.column};
            }
            break;
        case ']':
            tok = {TokenType::RBRACKET, std::string(1, m_char), tok.line, tok.column};
            break;
        case '@':
            tok = {TokenType::AT, std::string(1, m_char), tok.line, tok.column};
            break;
        case '.':
            tok = {TokenType::DOT, std::string(1, m_char), tok.line, tok.column};
            break;
        case '&':
            tok = {TokenType::AMPERSAND, std::string(1, m_char), tok.line, tok.column};
            break;
        case '"':
        case '\'':
            tok.type = TokenType::STRING;
            tok.literal = readString(m_char);
            return tok;
        case '#':
            if (peekChar() == ' ')
            {
                readChar();
                readChar();
                tok.type = TokenType::COMMENT;
                tok.literal = readComment();
                return tok;
            }
            else
            {
                tok = {TokenType::ILLEGAL, std::string(1, m_char), tok.line, tok.column};
                readChar();
                return tok;
            }
        case 0:
            tok.type = TokenType::END_OF_FILE;
            tok.literal = "";
            return tok;

        default:
            if (isdigit(m_char))
            {
                tok.type = TokenType::NUMBER;
                tok.literal = readNumber();
                return tok;
            }
            else if (isLetter(m_char))
            {
                tok.literal = readIdentifier();
                auto it = m_keywords.find(tok.literal);
                if (it != m_keywords.end())
                {
                    // It's a keyword. Use the token from the map, but keep the literal
                    // that was actually found in the source.
                    Token keyword_token = it->second;
                    keyword_token.literal = tok.literal;
                    tok = keyword_token;
                }
                else
                {
                    tok.type = TokenType::IDENT; // It's a regular identifier
                }
                return tok;
            }
            else
            {
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
        if (current_char == '{')
        {
            brace_level++;
        }
        else if (current_char == '}')
        {
            brace_level--;
        }
        scan_pos++;
    }

    if (brace_level > 0)
    {
        size_t end_pos = m_input.length();
        std::string content = m_input.substr(start_pos, end_pos - start_pos);
        while(m_readPosition < end_pos) { readChar(); }
        return content;
    }

    size_t end_pos = scan_pos - 1;
    std::string content;
    if (end_pos >= start_pos) {
        content = m_input.substr(start_pos, end_pos - start_pos);
    }

    while(m_position < end_pos) {
        readChar();
    }

    return content;
}

} // namespace CHTL
