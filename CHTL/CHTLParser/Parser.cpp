#include "Parser.h"
#include <memory>
#include <sstream>

namespace CHTL {

const std::map<TokenType, Precedence> Parser::precedences = {
    {TokenType::EQUAL_EQUAL, Precedence::EQUALS},
    {TokenType::NOT_EQUAL, Precedence::EQUALS},
    {TokenType::LESS_THAN, Precedence::LESSGREATER},
    {TokenType::GREATER_THAN, Precedence::LESSGREATER},
    {TokenType::PLUS, Precedence::SUM},
    {TokenType::MINUS, Precedence::SUM},
    {TokenType::SLASH, Precedence::PRODUCT},
    {TokenType::ASTERISK, Precedence::PRODUCT},
    {TokenType::QUESTION_MARK, Precedence::CONDITIONAL},
};

Parser::Parser(Lexer& lexer) : m_lexer(lexer), m_curToken({TokenType::ILLEGAL, "", 0, 0}), m_peekToken({TokenType::ILLEGAL, "", 0, 0}) {
    // Register prefix functions
    registerPrefix(TokenType::IDENTIFIER, &Parser::parseIdentifier);
    registerPrefix(TokenType::NUMERIC_LITERAL, &Parser::parseNumericLiteral);
    registerPrefix(TokenType::STRING_LITERAL, &Parser::parseStringLiteral);

    // Register infix functions
    registerInfix(TokenType::PLUS, &Parser::parseInfixExpression);
    registerInfix(TokenType::MINUS, &Parser::parseInfixExpression);
    registerInfix(TokenType::SLASH, &Parser::parseInfixExpression);
    registerInfix(TokenType::ASTERISK, &Parser::parseInfixExpression);
    registerInfix(TokenType::EQUAL_EQUAL, &Parser::parseInfixExpression);
    registerInfix(TokenType::NOT_EQUAL, &Parser::parseInfixExpression);
    registerInfix(TokenType::LESS_THAN, &Parser::parseInfixExpression);
    registerInfix(TokenType::GREATER_THAN, &Parser::parseInfixExpression);
    registerInfix(TokenType::QUESTION_MARK, &Parser::parseConditionalExpression);

    // Read two tokens, so m_curToken and m_peekToken are both set
    nextToken();
    nextToken();
}

void Parser::nextToken() {
    m_curToken = m_peekToken;
    m_peekToken = m_lexer.nextToken();
}

std::unique_ptr<ProgramNode> Parser::ParseProgram() {
    auto program = std::make_unique<ProgramNode>();

    while (m_curToken.type != TokenType::END_OF_FILE) {
        auto stmt = parseStatement();
        if (stmt) {
            program->AddChild(std::move(stmt));
        }
        nextToken();
    }

    return program;
}

std::unique_ptr<BaseNode> Parser::parseStatement() {
    if (m_curToken.type == TokenType::KEYWORD_TEXT) {
        return parseTextStatement();
    } else if (m_curToken.type == TokenType::IDENTIFIER && m_peekToken.type == TokenType::L_BRACE) {
        return parseElementStatement();
    }
    return nullptr;
}

std::unique_ptr<ElementNode> Parser::parseElementStatement() {
    auto elementNode = std::make_unique<ElementNode>(m_curToken);

    if (!expectPeek(TokenType::L_BRACE)) return nullptr;
    nextToken();

    while (m_curToken.type != TokenType::R_BRACE && m_curToken.type != TokenType::END_OF_FILE) {
        if (m_curToken.type == TokenType::KEYWORD_STYLE) {
            auto styleNode = parseStyleStatement();
            if (styleNode) elementNode->AddChild(std::move(styleNode));
        } else if (m_curToken.type == TokenType::IDENTIFIER && (m_peekToken.type == TokenType::COLON || m_peekToken.type == TokenType::EQUAL)) {
            auto attr = parseAttribute();
            if (attr) elementNode->AddAttribute(std::move(*attr));
        } else {
            auto stmt = parseStatement();
            if (stmt) elementNode->AddChild(std::move(stmt));
        }
        nextToken();
    }

    return elementNode;
}

std::unique_ptr<TextNode> Parser::parseTextStatement() {
    Token textToken = m_curToken;
    if (!expectPeek(TokenType::L_BRACE)) return nullptr;
    nextToken();

    std::stringstream ss;
    if (m_curToken.type == TokenType::STRING_LITERAL) {
        ss << m_curToken.literal;
        nextToken();
    } else {
        while (m_curToken.type != TokenType::R_BRACE && m_curToken.type != TokenType::END_OF_FILE) {
            ss << m_curToken.literal;
            nextToken();
            if (m_curToken.type != TokenType::R_BRACE) ss << " ";
        }
    }

    std::string textContent = ss.str();
    if (!textContent.empty() && textContent.back() == ' ') {
        textContent.pop_back();
    }

    if (m_curToken.type != TokenType::R_BRACE) {
        peekError(TokenType::R_BRACE);
        return nullptr;
    }

    return std::make_unique<TextNode>(textToken, textContent);
}

std::unique_ptr<StyleNode> Parser::parseStyleStatement() {
    auto styleNode = std::make_unique<StyleNode>(m_curToken);
    if (!expectPeek(TokenType::L_BRACE)) return nullptr;
    nextToken();

    while (m_curToken.type != TokenType::R_BRACE && m_curToken.type != TokenType::END_OF_FILE) {
        if (m_curToken.type == TokenType::IDENTIFIER) {
            auto prop = parseStylePropertyNode();
            if (prop) styleNode->AddChild(std::move(prop));
        }
        nextToken();
    }
    return styleNode;
}

std::unique_ptr<StylePropertyNode> Parser::parseStylePropertyNode() {
    Token propToken = m_curToken;
    std::string key = m_curToken.literal;

    if (!expectPeek(TokenType::COLON)) return nullptr;
    nextToken();

    auto value = parseExpression(Precedence::LOWEST);

    if (m_peekToken.type == TokenType::SEMICOLON) {
        nextToken();
    }

    return std::make_unique<StylePropertyNode>(propToken, key, std::move(value));
}

std::unique_ptr<Attribute> Parser::parseAttribute() {
    std::string key = m_curToken.literal;
    if (m_peekToken.type != TokenType::COLON && m_peekToken.type != TokenType::EQUAL) {
        peekError(TokenType::COLON);
        return nullptr;
    }
    nextToken();
    nextToken();

    auto value = parseExpression(Precedence::LOWEST);

    if (!expectPeek(TokenType::SEMICOLON)) {
        return nullptr;
    }

    return std::make_unique<Attribute>(key, std::move(value));
}

void Parser::registerPrefix(TokenType tokenType, prefixParseFn fn) { m_prefixParseFns[tokenType] = fn; }
void Parser::registerInfix(TokenType tokenType, infixParseFn fn) { m_infixParseFns[tokenType] = fn; }

Precedence Parser::peekPrecedence() {
    return precedences.count(m_peekToken.type) ? precedences.at(m_peekToken.type) : Precedence::LOWEST;
}
Precedence Parser::curPrecedence() {
    return precedences.count(m_curToken.type) ? precedences.at(m_curToken.type) : Precedence::LOWEST;
}

std::unique_ptr<ExpressionNode> Parser::parseExpression(Precedence precedence) {
    if (m_prefixParseFns.find(m_curToken.type) == m_prefixParseFns.end()) {
        m_errors.push_back("No prefix parse function for " + tokenTypeToString(m_curToken.type) + " found.");
        return nullptr;
    }
    auto prefix = m_prefixParseFns[m_curToken.type];
    auto leftExp = (this->*prefix)();

    while (m_peekToken.type != TokenType::SEMICOLON && m_peekToken.type != TokenType::R_BRACE && precedence < peekPrecedence()) {
        if (m_infixParseFns.find(m_peekToken.type) == m_infixParseFns.end()) {
            return leftExp;
        }
        auto infix = m_infixParseFns[m_peekToken.type];
        nextToken();
        leftExp = (this->*infix)(std::move(leftExp));
    }
    return leftExp;
}

std::unique_ptr<ExpressionNode> Parser::parseIdentifier() { return std::make_unique<LiteralNode>(m_curToken); }
std::unique_ptr<ExpressionNode> Parser::parseNumericLiteral() { return std::make_unique<LiteralNode>(m_curToken); }
std::unique_ptr<ExpressionNode> Parser::parseStringLiteral() { return std::make_unique<LiteralNode>(m_curToken); }

std::unique_ptr<ExpressionNode> Parser::parseInfixExpression(std::unique_ptr<ExpressionNode> left) {
    auto expression = std::make_unique<InfixExpressionNode>(m_curToken, std::move(left));
    auto precedence = curPrecedence();
    nextToken();
    expression->SetRight(parseExpression(precedence));
    return expression;
}

std::unique_ptr<ExpressionNode> Parser::parseConditionalExpression(std::unique_ptr<ExpressionNode> condition) {
    auto exp = std::make_unique<ConditionalExpressionNode>(m_curToken);
    exp->SetCondition(std::move(condition));
    nextToken();
    exp->SetConsequence(parseExpression(Precedence::LOWEST));
    if (!expectPeek(TokenType::COLON)) return nullptr;
    nextToken();
    exp->SetAlternative(parseExpression(Precedence::LOWEST));
    return exp;
}

bool Parser::expectPeek(TokenType t) {
    if (m_peekToken.type == t) {
        nextToken();
        return true;
    }
    peekError(t);
    return false;
}

void Parser::peekError(TokenType t) {
    std::string error = "Expected next token to be " + tokenTypeToString(t) + ", got " + tokenTypeToString(m_peekToken.type) + " instead.";
    m_errors.push_back(error);
}

} // namespace CHTL