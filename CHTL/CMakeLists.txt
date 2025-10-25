cmake_minimum_required(VERSION 3.20)
project(CHTL)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(chtl ../main.cpp)

target_include_directories(chtl PUBLIC
    ${PROJECT_SOURCE_DIR}/CHTL
)

# --- Tests ---
enable_testing()

add_executable(test_runner
    ../Test/main.cpp
    ../Test/CHTLJS/test_chtljs_lexer.cpp
    ../Test/CHTLJS/test_chtljs_lexer_arrow.cpp
    ../Test/CHTLJS/test_chtljs_lexer_bind.cpp
    ../Test/CHTLJS/test_chtljs_lexer_reactive.cpp
    ../Test/CHTLJS/test_chtljs_lexer_listen.cpp
    ../Test/CHTLJS/test_chtljs_parser.cpp
    ../Test/CHTLJS/test_chtljs_parser_reactive.cpp
    ../Test/CHTLJS/test_chtljs_parser_method_call.cpp
    ../CHTLJS/CHTLJSLexer/CHTLJSLexer.cpp
    ../CHTLJS/CHTLJSParser/CHTLJSParser.cpp
    ../CHTLJS/CHTLJSNode/EnhancedSelectorNode.cpp
    ../CHTLJS/CHTLJSNode/ReactiveValueNode.cpp
    ../CHTLJS/CHTLJSNode/MethodCallNode.cpp
)

target_include_directories(test_runner PUBLIC
    ${PROJECT_SOURCE_DIR}/Test
    ${PROJECT_SOURCE_DIR}/CHTL
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/CHTLJS
    ${PROJECT_SOURCE_DIR}/CHTLJS/CHTLJSParser
    ${PROJECT_SOURCE_DIR}/CHTLJS/CHTLJSNode
)

add_test(NAME AllTests COMMAND test_runner)
