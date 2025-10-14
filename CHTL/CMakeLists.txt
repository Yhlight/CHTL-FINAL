# CHTL/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

project(CHTL LANGUAGES CXX)

# 设置C++标准为20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置源文件和头文件的路径
set(CHTL_SOURCES
    src/Token.cpp
    src/Lexer.cpp
    src/AstNode.cpp
    src/Parser.cpp
    src/Generator.cpp
)
set(CHTL_HEADERS
    include/Token.h
    include/Lexer.h
    include/AstNode.h
    include/Parser.h
    include/Generator.h
)

# 添加库
add_library(CHTL STATIC ${CHTL_SOURCES} ${CHTL_HEADERS})

# 公开include目录
target_include_directories(CHTL PUBLIC include)

# 添加测试子目录
add_subdirectory(tests)
