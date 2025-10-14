// Parser 演示程序 - 展示如何使用 CHTL Parser

#include <iostream>
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"

using namespace CHTL;

// 简单的 AST 打印器
void printAST(const NodePtr& node, int indent = 0) {
    std::string indentStr(indent * 2, ' ');
    
    if (auto* element = dynamic_cast<ElementNode*>(node.get())) {
        std::cout << indentStr << "<" << element->getTagName();
        
        // 打印属性
        const auto& attrs = element->getAttributes();
        for (const auto& [key, value] : attrs) {
            std::cout << " " << key << "=\"" << value << "\"";
        }
        
        if (element->getChildCount() == 0) {
            std::cout << " />\n";
        } else {
            std::cout << ">\n";
            
            // 打印子节点
            for (const auto& child : element->getChildren()) {
                printAST(child, indent + 1);
            }
            
            std::cout << indentStr << "</" << element->getTagName() << ">\n";
        }
    } else if (auto* text = dynamic_cast<TextNode*>(node.get())) {
        std::cout << indentStr << "\"" << text->getContent() << "\"\n";
    }
}

int main() {
    // CHTL 源代码
    std::string source = R"(
        html {
            head {
                title: "CHTL Demo";
            }
            
            body {
                id: "main";
                class: "container";
                
                div {
                    class: "header";
                    
                    text {
                        "Welcome to CHTL"
                    }
                }
                
                div {
                    class: "content";
                    
                    span {
                        text: "Hello, World!";
                    }
                    
                    button {
                        text: "Click Me";
                    }
                }
            }
        }
    )";
    
    std::cout << "=== CHTL 源代码 ===\n" << source << "\n\n";
    
    // 词法分析
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    std::cout << "=== 词法分析 ===\n";
    std::cout << "Token 数量: " << tokens.size() << "\n\n";
    
    // 语法分析
    Parser parser(std::move(tokens));
    auto ast = parser.parse();
    
    if (parser.hasErrors()) {
        std::cout << "=== 解析错误 ===\n";
        for (const auto& error : parser.getErrors()) {
            std::cout << "错误: " << error << "\n";
        }
        return 1;
    }
    
    // 打印 AST
    std::cout << "=== 抽象语法树 (AST) ===\n";
    for (const auto& node : ast) {
        printAST(node);
    }
    
    return 0;
}
