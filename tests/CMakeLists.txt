# 启用测试
enable_testing()

# Catch2 测试框架（使用 single-header 版本）
add_library(catch2 INTERFACE)
target_include_directories(catch2 INTERFACE ${PROJECT_SOURCE_DIR}/third-party)

# 测试可执行文件
add_executable(chtl_tests
    test_main.cpp
    test_lexer.cpp
    test_parser.cpp
    test_generator.cpp
    test_production.cpp
    test_style.cpp
    test_expression.cpp
    test_script.cpp
    test_auto_class.cpp
    test_ampersand.cpp
    test_auto_head.cpp
)

target_link_libraries(chtl_tests PRIVATE 
    chtl_core
    catch2
)

# 添加测试
add_test(NAME chtl_tests COMMAND chtl_tests)
