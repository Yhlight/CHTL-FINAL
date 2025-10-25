cmake_minimum_required(VERSION 3.20)
project(CHTL)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(chtl
    src/CHTL/main.cpp
    src/CHTL/CHTLLexer/CHTLLexer.cpp
)
