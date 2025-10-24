cmake_minimum_required(VERSION 3.20)
project(CHTL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# CHTL Core Library
set(CHTL_CORE_SOURCES
    CHTL/CHTL/CHTLLexer/Lexer.cpp
    CHTL/CHTL/CHTLParser/Parser.cpp
    CHTL/CHTL/CHTLGenerator/Generator.cpp
)

add_library(chtl_core STATIC ${CHTL_CORE_SOURCES})
target_include_directories(chtl_core PUBLIC ${CMAKE_SOURCE_DIR})

# CHTL Compiler
add_executable(chtl CHTL/CHTL/main.cpp)
target_link_libraries(chtl PRIVATE chtl_core)

# Tests
enable_testing()

add_executable(lexer_test CHTL/Test/CHTLLexer/LexerTest.cpp)
target_link_libraries(lexer_test PRIVATE chtl_core)

add_executable(parser_test CHTL/Test/CHTLParser/ParserTest.cpp)
target_link_libraries(parser_test PRIVATE chtl_core)

add_executable(parser_features_test CHTL/Test/CHTLParser/ParserFeaturesTest.cpp)
target_link_libraries(parser_features_test PRIVATE chtl_core)

add_executable(parser_style_test CHTL/Test/CHTLParser/ParserStyleTest.cpp)
target_link_libraries(parser_style_test PRIVATE chtl_core)

add_executable(parser_global_style_test CHTL/Test/CHTLParser/ParserGlobalStyleTest.cpp)
target_link_libraries(parser_global_style_test PRIVATE chtl_core)

add_executable(parser_contextual_selector_test CHTL/Test/CHTLParser/ParserContextualSelectorTest.cpp)
target_link_libraries(parser_contextual_selector_test PRIVATE chtl_core)

add_executable(parser_attribute_arithmetic_test CHTL/Test/CHTLParser/ParserAttributeArithmeticTest.cpp)
target_link_libraries(parser_attribute_arithmetic_test PRIVATE chtl_core)
