#include "Lexer.h"
#include <cctype>

namespace CHTL
{

Lexer::Lexer(const std::string& input)
    : m_input(input)
{
    readChar(); // 初始化第一个字符
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

    // 更新行列号
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
    // readChar()会消耗掉第二个'/'
    while (m_char != '\n' && m_char != 0)
    {
        readChar();
    }
}

void Lexer::skipMultiLineComment()
{
    // readChar()会消耗掉'*'
    while (true)
    {
        if (m_char == '*' && peekChar() == '/')
        {
            readChar(); // 消耗 '*'
            readChar(); // 消耗 '/'
            break;
        }
        if (m_char == 0) // 文件提前结束
        {
            break;
        }
        readChar();
    }
}

// 检查字符是否是字母或下划线，这是标识符的开头
bool Lexer::isLetter(char ch)
{
    return isalpha(ch) || ch == '_';
}

// 读取一个完整的标识符
std::string Lexer::readIdentifier()
{
    size_t startPosition = m_position;
    // CHTL标识符允许字母、数字、下划线和连字符
    while (isLetter(m_char) || isdigit(m_char) || m_char == '-')
    {
        readChar();
    }
    return m_input.substr(startPosition, m_position - startPosition);
}

// 读取一个完整的字符串字面量
std::string Lexer::readString()
{
    size_t startPosition = m_position + 1; // 跳过起始的"
    do
    {
        readChar();
        // 可以在这里添加对转义字符的支持
    } while (m_char != '"' && m_char != 0);

    std::string result = m_input.substr(startPosition, m_position - startPosition);
    readChar(); // 消耗结束的"
    return result;
}

// 读取一个完整的数字
std::string Lexer::readNumber()
{
    size_t startPosition = m_position;
    while (isdigit(m_char) || m_char == '.')
    {
        readChar();
    }
    return m_input.substr(startPosition, m_position - startPosition);
}


Token Lexer::NextToken()
{
    Token tok;

    skipWhitespace();

    // 记录Token的起始位置
    tok.line = m_line;
    tok.column = m_column;

    switch (m_char)
    {
        case '=':
            tok = {TokenType::ASSIGN, std::string(1, m_char), tok.line, tok.column};
            break;
        case ':':
            tok = {TokenType::COLON, std::string(1, m_char), tok.line, tok.column};
            break;
        case ';':
            tok = {TokenType::SEMICOLON, std::string(1, m_char), tok.line, tok.column};
            break;
        case '{':
            tok = {TokenType::LBRACE, std::string(1, m_char), tok.line, tok.column};
            break;
        case '}':
            tok = {TokenType::RBRACE, std::string(1, m_char), tok.line, tok.column};
            break;
        case '"':
            tok.type = TokenType::STRING;
            tok.literal = readString();
            // readString 更新了行列号，所以不需要额外处理
            return tok;
        case '/':
            if (peekChar() == '/')
            {
                readChar(); // 消耗第一个'/'
                readChar(); // 消耗第二个'/'
                skipSingleLineComment();
                return NextToken(); // 递归调用以获取下一个有效Token
            }
            else if (peekChar() == '*')
            {
                readChar(); // 消耗'/'
                readChar(); // 消耗'*'
                skipMultiLineComment();
                return NextToken(); // 递归调用以获取下一个有效Token
            }
            // 如果不是注释，暂时作为非法字符处理 (后续会是除号)
            tok = {TokenType::ILLEGAL, std::string(1, m_char), tok.line, tok.column};
            break;

        case 0:
            tok.type = TokenType::END_OF_FILE;
            tok.literal = "";
            return tok; // 直接返回EOF

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
                auto it = keywords.find(tok.literal);
                if (it != keywords.end())
                {
                    tok.type = it->second; // 是关键字
                }
                else
                {
                    tok.type = TokenType::IDENT; // 是普通标识符
                }
                // readIdentifier 更新了行列号，所以不需要额外处理
                return tok;
            }
            else
            {
                tok = {TokenType::ILLEGAL, std::string(1, m_char), tok.line, tok.column};
            }
            break;
    }

    readChar(); // 消耗当前字符，为下一次调用做准备
    return tok;
}

} // namespace CHTL
