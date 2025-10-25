#ifndef CHTLJS_LEXER_HPP
#define CHTLJS_LEXER_HPP

#include <string>
#include <vector>

enum class CHTLJSTokenType {
    ENHANCED_SELECTOR,
    ARROW_OPERATOR,
    BIND_OPERATOR,
    REACTIVE_VALUE,
    IDENTIFIER,
    KEYWORD_LISTEN,
    KEYWORD_DELEGATE,
    KEYWORD_ANIMATE,
    KEYWORD_VIR,
    KEYWORD_ROUTER,
    KEYWORD_SCRIPTLOADER,
    UNKNOWN,
};

struct CHTLJSToken {
    CHTLJSTokenType type;
    std::string value;
    size_t start_pos;
    size_t end_pos;
};

class CHTLJSLexer {
public:
    CHTLJSLexer(const std::string& input);
    std::vector<CHTLJSToken> tokenize();

private:
    std::string input_;
    size_t position_ = 0;
};

#endif // CHTLJS_LEXER_HPP
