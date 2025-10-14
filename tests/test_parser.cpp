#include "catch.hpp"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"

using namespace CHTL;

// 辅助函数：从源代码创建 AST
std::vector<NodePtr> parseSource(const std::string& source) {
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    Parser parser(std::move(tokens));
    return parser.parse();
}

TEST_CASE("Parser - 简单元素解析", "[parser]") {
    SECTION("解析空的 div 元素") {
        std::string code = "div { }";
        auto ast = parseSource(code);
        
        REQUIRE(ast.size() == 1);
        REQUIRE(ast[0]->getType() == NodeType::ELEMENT);
        
        auto* element = dynamic_cast<ElementNode*>(ast[0].get());
        REQUIRE(element != nullptr);
        REQUIRE(element->getTagName() == "div");
        REQUIRE(element->getChildCount() == 0);
    }
    
    SECTION("解析带标签名的元素") {
        std::string code = "span { }";
        auto ast = parseSource(code);
        
        REQUIRE(ast.size() == 1);
        auto* element = dynamic_cast<ElementNode*>(ast[0].get());
        REQUIRE(element != nullptr);
        REQUIRE(element->getTagName() == "span");
    }
    
    SECTION("解析多个元素") {
        std::string code = R"(
            div { }
            span { }
            button { }
        )";
        auto ast = parseSource(code);
        
        REQUIRE(ast.size() == 3);
        REQUIRE(dynamic_cast<ElementNode*>(ast[0].get())->getTagName() == "div");
        REQUIRE(dynamic_cast<ElementNode*>(ast[1].get())->getTagName() == "span");
        REQUIRE(dynamic_cast<ElementNode*>(ast[2].get())->getTagName() == "button");
    }
}

TEST_CASE("Parser - 元素属性解析", "[parser]") {
    SECTION("解析单个属性 - 使用冒号") {
        std::string code = R"(
            div {
                id: "container";
            }
        )";
        auto ast = parseSource(code);
        
        REQUIRE(ast.size() == 1);
        auto* element = dynamic_cast<ElementNode*>(ast[0].get());
        REQUIRE(element != nullptr);
        REQUIRE(element->hasAttribute("id"));
        REQUIRE(element->getAttribute("id") == "container");
    }
    
    SECTION("解析单个属性 - 使用等号 (CE对等式)") {
        std::string code = R"(
            div {
                class = "box";
            }
        )";
        auto ast = parseSource(code);
        
        auto* element = dynamic_cast<ElementNode*>(ast[0].get());
        REQUIRE(element->hasAttribute("class"));
        REQUIRE(element->getAttribute("class") == "box");
    }
    
    SECTION("解析多个属性") {
        std::string code = R"(
            div {
                id: "main";
                class: "container";
                data-value: "123";
            }
        )";
        auto ast = parseSource(code);
        
        auto* element = dynamic_cast<ElementNode*>(ast[0].get());
        REQUIRE(element->hasAttribute("id"));
        REQUIRE(element->hasAttribute("class"));
        REQUIRE(element->hasAttribute("data-value"));
        REQUIRE(element->getAttribute("id") == "main");
        REQUIRE(element->getAttribute("class") == "container");
        REQUIRE(element->getAttribute("data-value") == "123");
    }
    
    SECTION("解析不带分号的属性") {
        std::string code = R"(
            div {
                id: "test"
            }
        )";
        auto ast = parseSource(code);
        
        auto* element = dynamic_cast<ElementNode*>(ast[0].get());
        REQUIRE(element->hasAttribute("id"));
        REQUIRE(element->getAttribute("id") == "test");
    }
}

TEST_CASE("Parser - 文本节点解析", "[parser]") {
    SECTION("解析 text 块") {
        std::string code = R"(
            div {
                text {
                    "Hello World"
                }
            }
        )";
        auto ast = parseSource(code);
        
        REQUIRE(ast.size() == 1);
        auto* element = dynamic_cast<ElementNode*>(ast[0].get());
        REQUIRE(element != nullptr);
        REQUIRE(element->getChildCount() == 1);
        
        auto* textNode = dynamic_cast<TextNode*>(element->getChildren()[0].get());
        REQUIRE(textNode != nullptr);
        REQUIRE(textNode->getContent() == "Hello World");
    }
    
    SECTION("解析文本属性") {
        std::string code = R"(
            div {
                text: "Hello CHTL";
            }
        )";
        auto ast = parseSource(code);
        
        auto* element = dynamic_cast<ElementNode*>(ast[0].get());
        REQUIRE(element->hasAttribute("text"));
        REQUIRE(element->getAttribute("text") == "Hello CHTL");
    }
}

TEST_CASE("Parser - 嵌套元素解析", "[parser]") {
    SECTION("解析简单嵌套") {
        std::string code = R"(
            div {
                span { }
            }
        )";
        auto ast = parseSource(code);
        
        REQUIRE(ast.size() == 1);
        auto* div = dynamic_cast<ElementNode*>(ast[0].get());
        REQUIRE(div != nullptr);
        REQUIRE(div->getChildCount() == 1);
        
        auto* span = dynamic_cast<ElementNode*>(div->getChildren()[0].get());
        REQUIRE(span != nullptr);
        REQUIRE(span->getTagName() == "span");
    }
    
    SECTION("解析多层嵌套") {
        std::string code = R"(
            html {
                body {
                    div {
                        span { }
                    }
                }
            }
        )";
        auto ast = parseSource(code);
        
        REQUIRE(ast.size() == 1);
        auto* html = dynamic_cast<ElementNode*>(ast[0].get());
        REQUIRE(html->getTagName() == "html");
        REQUIRE(html->getChildCount() == 1);
        
        auto* body = dynamic_cast<ElementNode*>(html->getChildren()[0].get());
        REQUIRE(body->getTagName() == "body");
        REQUIRE(body->getChildCount() == 1);
        
        auto* div = dynamic_cast<ElementNode*>(body->getChildren()[0].get());
        REQUIRE(div->getTagName() == "div");
        REQUIRE(div->getChildCount() == 1);
        
        auto* span = dynamic_cast<ElementNode*>(div->getChildren()[0].get());
        REQUIRE(span->getTagName() == "span");
    }
    
    SECTION("解析同级多个子元素") {
        std::string code = R"(
            div {
                span { }
                button { }
                input { }
            }
        )";
        auto ast = parseSource(code);
        
        auto* div = dynamic_cast<ElementNode*>(ast[0].get());
        REQUIRE(div->getChildCount() == 3);
        REQUIRE(dynamic_cast<ElementNode*>(div->getChildren()[0].get())->getTagName() == "span");
        REQUIRE(dynamic_cast<ElementNode*>(div->getChildren()[1].get())->getTagName() == "button");
        REQUIRE(dynamic_cast<ElementNode*>(div->getChildren()[2].get())->getTagName() == "input");
    }
}

TEST_CASE("Parser - 完整示例解析", "[parser][integration]") {
    SECTION("解析带属性和文本的完整元素") {
        std::string code = R"(
            div {
                id: "container";
                class: "main-content";
                
                text {
                    "Welcome to CHTL"
                }
                
                span {
                    text: "Nested content";
                }
            }
        )";
        auto ast = parseSource(code);
        
        REQUIRE(ast.size() == 1);
        auto* div = dynamic_cast<ElementNode*>(ast[0].get());
        REQUIRE(div != nullptr);
        REQUIRE(div->getTagName() == "div");
        REQUIRE(div->hasAttribute("id"));
        REQUIRE(div->hasAttribute("class"));
        REQUIRE(div->getChildCount() == 2); // text 块 + span 元素
    }
}

TEST_CASE("Parser - 特殊块跳过", "[parser]") {
    SECTION("跳过 style 块") {
        std::string code = R"(
            div {
                style {
                    width: 100px;
                    height: 100px;
                }
            }
        )";
        auto ast = parseSource(code);
        
        REQUIRE(ast.size() == 1);
        auto* div = dynamic_cast<ElementNode*>(ast[0].get());
        REQUIRE(div != nullptr);
        // style 块当前被跳过，不作为子节点
    }
    
    SECTION("跳过 script 块") {
        std::string code = R"(
            div {
                script {
                    console.log("test");
                }
            }
        )";
        auto ast = parseSource(code);
        
        REQUIRE(ast.size() == 1);
        auto* div = dynamic_cast<ElementNode*>(ast[0].get());
        REQUIRE(div != nullptr);
        // script 块当前被跳过，不作为子节点
    }
}
