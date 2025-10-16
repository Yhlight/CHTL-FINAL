#include "Lexer.h"
#include <cctype>
#include <unordered_map>

namespace CHTL
{

// 关键字映射表
static const std::unordered_map<std::string, TokenType> keywords = {
    {"text", TokenType::KEYWORD_TEXT},
    {"style", TokenType::KEYWORD_STYLE},
    {"script", TokenType::KEYWORD_SCRIPT},
    {"inherit", TokenType::KEYWORD_INHERIT},
    {"delete", TokenType::KEYWORD_DELETE},
    {"insert", TokenType::KEYWORD_INSERT},
    {"after", TokenType::KEYWORD_AFTER},
    {"before", TokenType::KEYWORD_BEFORE},
    {"replace", TokenType::KEYWORD_REPLACE},
    {"at", TokenType::KEYWORD_AT}, // for "at top", "at bottom"
    {"top", TokenType::KEYWORD_TOP},
    {"bottom", TokenType::KEYWORD_BOTTOM},
    {"from", TokenType::KEYWORD_FROM},
    {"as", TokenType::KEYWORD_AS},
    {"except", TokenType::KEYWORD_EXCEPT},
    {"use", TokenType::KEYWORD_USE},
    {"html5", TokenType::KEYWORD_HTML5},
};

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

// 读取一个块级关键字，如 [Template] 或 [Custom]
Token Lexer::readBlockKeyword()
{
    int startLine = m_line;
    int startColumn = m_column;

    readChar(); // 消耗 '['

    std::string identifier = readIdentifier();

    if (m_char != ']')
    {
        // 格式错误，这不是一个合法的块关键字
        // 我们需要回溯状态，但这会使lexer变复杂。
        // 一个简单的处理方法是，返回ILLEGAL，让解析器处理错误。
        // 这里我们先简单处理，假设输入总是合法的。
        return {TokenType::ILLEGAL, "[" + identifier, startLine, startColumn};
    }

    readChar(); // 消耗 ']'

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

    return {TokenType::ILLEGAL, "[" + identifier + "]", startLine, startColumn};
}

// 读取一个完整的字符串字面量
std::string Lexer::readString(char quote_type)
{
    size_t startPosition = m_position + 1; // 跳过起始的引号
    do
    {
        readChar();
        // 可以在这里添加对转义字符的支持
    } while (m_char != quote_type && m_char != 0);

    std::string result = m_input.substr(startPosition, m_position - startPosition);
    readChar(); // 消耗结束的引号
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

// 读取生成器注释 (假设 '#' 和 ' ' 已经被消耗)
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

    // 记录Token的起始位置
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
            // 检查这是否是一个块级关键字的开始
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
            // readString 更新了行列号，所以不需要额外处理
            return tok;
        case '#':
            if (peekChar() == ' ')
            {
                readChar(); // 消耗 '#'
                readChar(); // 消耗 ' '
                tok.type = TokenType::COMMENT;
                tok.literal = readComment();
                return tok;
            }
            else
            {
                // '#` 后面没有空格，这是一个非法的Token
                tok = {TokenType::ILLEGAL, std::string(1, m_char), tok.line, tok.column};
                readChar(); // 消耗 '#'
                return tok;
            }
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
