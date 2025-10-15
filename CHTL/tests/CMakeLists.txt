# CHTL/tests/CMakeLists.txt

# 添加测试可执行文件
add_executable(TokenTest TokenTest.cpp)
add_executable(AstTest AstTest.cpp)
add_executable(GeneratorTest GeneratorTest.cpp)
add_executable(EvaluatorTest EvaluatorTest.cpp)
add_executable(LexerTest LexerTest.cpp)

# 链接CHTL库
target_link_libraries(TokenTest PRIVATE CHTL)
target_link_libraries(AstTest PRIVATE CHTL)
target_link_libraries(GeneratorTest PRIVATE CHTL)
target_link_libraries(EvaluatorTest PRIVATE CHTL)
target_link_libraries(LexerTest PRIVATE CHTL)

# 添加third-party目录以便找到catch.hpp
target_include_directories(TokenTest PRIVATE ../../third-party)
target_include_directories(AstTest PRIVATE ../../third-party)
target_include_directories(GeneratorTest PRIVATE ../../third-party)
target_include_directories(EvaluatorTest PRIVATE ../../third-party)
target_include_directories(LexerTest PRIVATE ../../third-party)
