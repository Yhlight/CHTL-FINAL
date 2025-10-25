cmake_minimum_required(VERSION 3.20)
project(CHTL)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(chtl CHTL/main.cpp)

target_include_directories(chtl PUBLIC
    ${PROJECT_SOURCE_DIR}/CHTL
    ${PROJECT_SOURCE_DIR}/CHTL/CHTLLexer
)

# --- Tests ---
enable_testing()

add_executable(test_runner
    Test/main.cpp
    Test/test_lexer.cpp
    Test/test_parser.cpp
    Test/test_generator.cpp
    CHTL/CHTLLexer/CHTLLexer.cpp
    CHTL/CHTLParser/CHTLParser.cpp
    CHTL/CHTLNode/ElementNode.cpp
    CHTL/CHTLNode/TextNode.cpp
    CHTL/CHTLNode/PropertyNode.cpp
    CHTL/CHTLNode/RuleNode.cpp
    CHTL/CHTLGenerator/CHTLGenerator.cpp
)

target_include_directories(test_runner PUBLIC
    ${PROJECT_SOURCE_DIR}/Test
    ${PROJECT_SOURCE_DIR}/CHTL
)

add_test(NAME AllTests COMMAND test_runner)
