# CHTL/tests/CMakeLists.txt

# 添加测试可执行文件
add_executable(TokenTest TokenTest.cpp)
add_executable(AstTest AstTest.cpp)

# 链接CHTL库
target_link_libraries(TokenTest PRIVATE CHTL)
target_link_libraries(AstTest PRIVATE CHTL)

# 添加third-party目录以便找到catch.hpp
target_include_directories(TokenTest PRIVATE ../../third-party)
target_include_directories(AstTest PRIVATE ../../third-party)
