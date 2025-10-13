#pragma once

#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/Node.h"
#include "../CHTLNode/Statement.h"
#include "../CHTLNode/Expression.h"
#include "../CHTLNode/Style.h"
#include <vector>
#include <string>
#include <map>
#include <functional>

namespace CHTL {

    enum class Precedence {
        LOWEST,
        SUM,         // + or -
        PRODUCT,     // * or / or %
        POWER,       // **
        PREFIX,      // -X or !X
    };

    extern const std::map<TokenType, Precedence> precedences;

    class Parser; // Forward declaration

    using PrefixParseFn = std::function<std::unique_ptr<Expression>(Parser*)>;
    using InfixParseFn = std::function<std::unique_ptr<Expression>(Parser*, std::unique_ptr<Expression>)>;

    extern const std::map<TokenType, PrefixParseFn> prefixParseFns;
    extern const std::map<TokenType, InfixParseFn> infixParseFns;


    class Parser {
    public:
        Parser(Lexer& l);
        std::unique_ptr<Program> ParseProgram();
        const std::vector<std::string>& Errors() const { return errors; }

        friend std::unique_ptr<Expression> ParseInfixExpression(Parser* parser, std::unique_ptr<Expression> left);
        friend std::unique_ptr<Expression> ParseIdentifier(Parser* parser);
        friend std::unique_ptr<Expression> ParseNumberLiteral(Parser* parser);
        friend std::unique_ptr<Expression> ParseStringLiteral(Parser* parser);

    private:
        void NextToken();
        void PeekError(TokenType t);

        std::unique_ptr<Statement> ParseStatement();
        std::unique_ptr<ElementStatement> ParseElementStatement();
        std::unique_ptr<Attribute> ParseAttribute();
        std::unique_ptr<StyleStatement> ParseStyleStatement();
        std::unique_ptr<StyleProperty> ParseStyleProperty();
        std::unique_ptr<TextStatement> ParseTextStatement();

        std::unique_ptr<Expression> ParseExpression(Precedence precedence);
        std::unique_ptr<Expression> ParseIdentifier();
        std::unique_ptr<Expression> ParseNumberLiteral();
        std::unique_ptr<Expression> ParseStringLiteral();
        std::unique_ptr<Expression> ParseUnquotedLiteral();

        std::unique_ptr<BlockStatement> ParseBlockStatement();

        Precedence PeekPrecedence();
        Precedence CurPrecedence();

        Lexer& lexer;
        Token currentToken;
        Token peekToken;

        std::vector<std::string> errors;
    };

} // namespace CHTL
