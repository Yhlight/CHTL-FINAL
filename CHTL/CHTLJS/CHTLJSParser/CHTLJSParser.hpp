#ifndef CHTLJS_PARSER_HPP
#define CHTLJS_PARSER_HPP

#include "CHTLJSLexer/CHTLJSLexer.hpp"
#include "CHTLJSNode/CHTLJSNode.hpp"
#include "CHTLJSNode/EnhancedSelectorNode.hpp"
#include "CHTLJSNode/MethodCallNode.hpp"
#include <vector>
#include <memory>

class CHTLJSParser {
public:
    CHTLJSParser(CHTLJSLexer& lexer);
    std::unique_ptr<CHTLJSNode> parse();

private:
    std::unique_ptr<CHTLJSNode> parseExpression();
    std::unique_ptr<CHTLJSNode> parsePrimary();

    std::vector<CHTLJSToken> tokens_;
    size_t position_ = 0;
};

#endif // CHTLJS_PARSER_HPP
