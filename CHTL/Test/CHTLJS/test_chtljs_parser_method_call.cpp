#include "CustomTestFramework.hpp"
#include "CHTLJS/CHTLJSParser/CHTLJSParser.hpp"
#include "CHTLJSLexer/CHTLJSLexer.hpp"
#include "CHTLJSNode/MethodCallNode.hpp"
#include "CHTLJSNode/EnhancedSelectorNode.hpp"

TEST_CASE("CHTLJS Parser can parse a method call") {
    std::string input = "{{.box}}->textContent";
    CHTLJSLexer lexer(input);
    CHTLJSParser parser(lexer);
    std::unique_ptr<CHTLJSNode> root = parser.parse();
    REQUIRE(root != nullptr);
    auto methodCallNode = dynamic_cast<MethodCallNode*>(root.get());
    REQUIRE(methodCallNode != nullptr);
    auto selectorNode = dynamic_cast<EnhancedSelectorNode*>(methodCallNode->object.get());
    REQUIRE(selectorNode != nullptr);
    REQUIRE(selectorNode->selector == ".box");
    REQUIRE(methodCallNode->method_name == "textContent");
}
