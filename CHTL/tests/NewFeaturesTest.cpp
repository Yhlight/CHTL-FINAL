#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "CHTLJS/include/parser/Parser.h"
#include "CHTLJS/include/lexer/Lexer.h"
#include "CHTLJS/include/generator/Generator.h"
#include "CHTLJS/include/nodes/AstNode.h"
#include "CHTLJS/include/nodes/EventBindNode.h"
#include "CHTLJS/include/nodes/RouterNode.h"
#include "CHTLJS/include/nodes/ReactiveValueNode.h"
#include "bridge/ConcreteSaltBridge.h"
#include <vector>
#include <string>
#include <memory>

// Helper function to check for parser errors
void checkParserErrors(const CHTLJS::Parser& p) {
    const auto& errors = p.GetErrors();
    if (errors.empty()) {
        return;
    }

    std::string error_message = "Parser has " + std::to_string(errors.size()) + " errors:\n";
    for (const auto& msg : errors) {
        error_message += "\t" + msg + "\n";
    }
    FAIL(error_message);
}

TEST_CASE("Test EventBind Node creation", "[eventbind]")
{
    auto node = std::make_unique<CHTLJS::EventBindNode>();
    node->eventNames = {"click", "mouseenter"};
    node->handler = "handleClick";
    node->isBlock = false;

    REQUIRE(node->eventNames.size() == 2);
    REQUIRE(node->eventNames[0] == "click");
    REQUIRE(node->eventNames[1] == "mouseenter");
    REQUIRE(node->handler == "handleClick");
    REQUIRE(node->isBlock == false);
    
    // Test clone
    auto cloned = node->clone();
    auto clonedBind = dynamic_cast<CHTLJS::EventBindNode*>(cloned.get());
    REQUIRE(clonedBind != nullptr);
    REQUIRE(clonedBind->eventNames.size() == 2);
}

TEST_CASE("Test Router Node creation", "[router]")
{
    auto node = std::make_unique<CHTLJS::RouterNode>();
    node->urlPageMappings.push_back({"/home", "#home-page"});
    node->urlPageMappings.push_back({"/about", "#about-page"});
    node->mode = "history";
    node->rootPath = "/";

    REQUIRE(node->urlPageMappings.size() == 2);
    REQUIRE(node->urlPageMappings[0].first == "/home");
    REQUIRE(node->urlPageMappings[0].second == "#home-page");
    REQUIRE(node->mode == "history");
    REQUIRE(node->rootPath == "/");
    
    // Test clone
    auto cloned = node->clone();
    auto clonedRouter = dynamic_cast<CHTLJS::RouterNode*>(cloned.get());
    REQUIRE(clonedRouter != nullptr);
    REQUIRE(clonedRouter->urlPageMappings.size() == 2);
}

TEST_CASE("Test ReactiveValue Node creation", "[reactive]")
{
    auto node = std::make_unique<CHTLJS::ReactiveValueNode>("myVariable");

    REQUIRE(node->variableName == "myVariable");
    
    // Test clone
    auto cloned = node->clone();
    auto clonedReactive = dynamic_cast<CHTLJS::ReactiveValueNode*>(cloned.get());
    REQUIRE(clonedReactive != nullptr);
    REQUIRE(clonedReactive->variableName == "myVariable");
}

TEST_CASE("Test EventBind code generation", "[eventbind][generator]")
{
    auto bridge = std::make_shared<CHTL::ConcreteSaltBridge>();
    CHTLJS::Generator generator(bridge);

    auto program = std::make_unique<CHTLJS::ProgramNode>();
    
    // Create event bind node
    auto eventBind = std::make_unique<CHTLJS::EventBindNode>();
    auto selector = std::make_unique<CHTLJS::EnhancedSelectorNode>();
    selector->selector = ".box";
    eventBind->target = std::move(selector);
    eventBind->eventNames = {"click"};
    eventBind->handler = "() => { console.log('clicked'); }";
    eventBind->isBlock = false;
    
    program->children.push_back(std::move(eventBind));

    std::string output = generator.Generate(*program);
    
    // Check that the output contains addEventListener
    REQUIRE(output.find("addEventListener") != std::string::npos);
    REQUIRE(output.find("click") != std::string::npos);
}

TEST_CASE("Test Router code generation", "[router][generator]")
{
    auto bridge = std::make_shared<CHTL::ConcreteSaltBridge>();
    CHTLJS::Generator generator(bridge);

    auto program = std::make_unique<CHTLJS::ProgramNode>();
    
    // Create router node
    auto router = std::make_unique<CHTLJS::RouterNode>();
    router->urlPageMappings.push_back({"/home", "#home-page"});
    router->urlPageMappings.push_back({"/about", "#about-page"});
    router->mode = "history";
    
    program->children.push_back(std::move(router));

    std::string output = generator.Generate(*program);
    
    // Check that the output contains routing logic
    REQUIRE(output.find("routes") != std::string::npos);
    REQUIRE(output.find("/home") != std::string::npos);
    REQUIRE(output.find("#home-page") != std::string::npos);
    REQUIRE(output.find("navigate") != std::string::npos);
}

TEST_CASE("Test ReactiveValue code generation", "[reactive][generator]")
{
    auto bridge = std::make_shared<CHTL::ConcreteSaltBridge>();
    CHTLJS::Generator generator(bridge);

    auto program = std::make_unique<CHTLJS::ProgramNode>();
    
    // Create reactive value node
    auto reactive = std::make_unique<CHTLJS::ReactiveValueNode>("boxWidth");
    program->children.push_back(std::move(reactive));

    std::string output = generator.Generate(*program);
    
    // Check that the output contains the variable name
    REQUIRE(output.find("boxWidth") != std::string::npos);
}

TEST_CASE("Test comprehensive CHTL JS features", "[comprehensive]")
{
    auto bridge = std::make_shared<CHTL::ConcreteSaltBridge>();
    CHTLJS::Generator generator(bridge);

    auto program = std::make_unique<CHTLJS::ProgramNode>();
    
    // Add enhanced selector
    auto selector = std::make_unique<CHTLJS::EnhancedSelectorNode>();
    selector->selector = ".container";
    program->children.push_back(std::move(selector));
    
    // Add router
    auto router = std::make_unique<CHTLJS::RouterNode>();
    router->urlPageMappings.push_back({"/", "#main"});
    router->mode = "hash";
    program->children.push_back(std::move(router));

    std::string output = generator.Generate(*program);
    
    // Verify multiple features work together
    REQUIRE(output.find("querySelector") != std::string::npos);
    REQUIRE(output.find("routes") != std::string::npos);
    REQUIRE(!output.empty());
}
