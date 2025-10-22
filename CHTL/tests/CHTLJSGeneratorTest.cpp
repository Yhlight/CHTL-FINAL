#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "parser/Parser.h"
#include "generator/Generator.h"
#include "bridge/ConcreteSaltBridge.h"

TEST_CASE("CHTLJS Generator with parent context", "[CHTLJSGenerator]") {
    std::string chtl_code = R"(
        div {
            id: "my-div";
            script {
                {{&}}.innerHTML = "Hello from CHTLJS";
            }
        }
    )";

    CHTL::Lexer lexer(chtl_code);
    auto bridge = std::make_shared<CHTL::ConcreteSaltBridge>();
    CHTL::Parser parser(lexer);
    parser.SetBridge(bridge);
    auto program = parser.ParseProgram();

    REQUIRE(parser.GetErrors().empty());

    CHTL::Generator generator(bridge);
    std::string result = generator.Generate(program.get());

    std::string expected_js = "document.querySelector('#my-div').innerHTML = \"Hello from CHTLJS\";";
    REQUIRE(result.find(expected_js) != std::string::npos);
}
