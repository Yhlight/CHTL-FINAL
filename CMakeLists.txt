cmake_minimum_required(VERSION 3.20)
project(CHTL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# CHTL Compiler sources
set(CHTL_COMPILER_SOURCES
    CHTL/CHTL/main.cpp
    CHTL/CHTL/CHTLLexer/Lexer.cpp
    CHTL/CHTL/CHTLParser/Parser.cpp
    CHTL/CHTL/CHTLGenerator/Generator.cpp
)

add_executable(chtl ${CHTL_COMPILER_SOURCES})
target_include_directories(chtl PUBLIC ${CMAKE_SOURCE_DIR})

# Tests
enable_testing()

set(LEXER_TEST_SOURCES
    CHTL/Test/CHTLLexer/LexerTest.cpp
    CHTL/CHTL/CHTLLexer/Lexer.cpp
)

add_executable(lexer_test ${LEXER_TEST_SOURCES})
target_include_directories(lexer_test PUBLIC ${CMAKE_SOURCE_DIR})

set(PARSER_TEST_SOURCES
    CHTL/Test/CHTLParser/ParserTest.cpp
    CHTL/CHTL/CHTLLexer/Lexer.cpp
    CHTL/CHTL/CHTLParser/Parser.cpp
)

add_executable(parser_test ${PARSER_TEST_SOURCES})
target_include_directories(parser_test PUBLIC ${CMAKE_SOURCE_DIR})

set(PARSER_FEATURES_TEST_SOURCES
    CHTL/Test/CHTLParser/ParserFeaturesTest.cpp
    CHTL/CHTL/CHTLLexer/Lexer.cpp
    CHTL/CHTL/CHTLParser/Parser.cpp
    CHTL/CHTL/CHTLGenerator/Generator.cpp
)

add_executable(parser_features_test ${PARSER_FEATURES_TEST_SOURCES})
target_include_directories(parser_features_test PUBLIC ${CMAKE_SOURCE_DIR})

set(PARSER_STYLE_TEST_SOURCES
    CHTL/Test/CHTLParser/ParserStyleTest.cpp
    CHTL/CHTL/CHTLLexer/Lexer.cpp
    CHTL/CHTL/CHTLParser/Parser.cpp
    CHTL/CHTL/CHTLGenerator/Generator.cpp
)

add_executable(parser_style_test ${PARSER_STYLE_TEST_SOURCES})
target_include_directories(parser_style_test PUBLIC ${CMAKE_SOURCE_DIR})

set(PARSER_GLOBAL_STYLE_TEST_SOURCES
    CHTL/Test/CHTLParser/ParserGlobalStyleTest.cpp
    CHTL/CHTL/CHTLLexer/Lexer.cpp
    CHTL/CHTL/CHTLParser/Parser.cpp
    CHTL/CHTL/CHTLGenerator/Generator.cpp
)

add_executable(parser_global_style_test ${PARSER_GLOBAL_STYLE_TEST_SOURCES})
target_include_directories(parser_global_style_test PUBLIC ${CMAKE_SOURCE_DIR})

set(PARSER_CONTEXTUAL_SELECTOR_TEST_SOURCES
    CHTL/Test/CHTLParser/ParserContextualSelectorTest.cpp
    CHTL/CHTL/CHTLLexer/Lexer.cpp
    CHTL/CHTL/CHTLParser/Parser.cpp
    CHTL/CHTL/CHTLGenerator/Generator.cpp
)

add_executable(parser_contextual_selector_test ${PARSER_CONTEXTUAL_SELECTOR_TEST_SOURCES})
target_include_directories(parser_contextual_selector_test PUBLIC ${CMAKE_SOURCE_DIR})
