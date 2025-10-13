#pragma once

#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/Node.h"
#include "../CHTLNode/Statement.h"
#include "../CHTLNode/Expression.h"
#include <vector>
#include <string>

namespace CHTL {

    class Parser {
    public:
        Parser(Lexer& l);
        std::unique_ptr<Program> ParseProgram();
        const std::vector<std::string>& Errors() const { return errors; }

    private:
        void NextToken();
        void PeekError(TokenType t);

        std::unique_ptr<Statement> ParseStatement();
        std::unique_ptr<ElementStatement> ParseElementStatement();
        std::unique_ptr<Attribute> ParseAttribute();
        std::unique_ptr<TextStatement> ParseTextStatement();
        std::unique_ptr<Expression> ParseUnquotedLiteral();
        std::unique_ptr<Expression> ParseExpression();
        std::unique_ptr<BlockStatement> ParseBlockStatement();

        Lexer& lexer;
        Token currentToken;
        Token peekToken;

        std::vector<std::string> errors;
    };

} // namespace CHTL
