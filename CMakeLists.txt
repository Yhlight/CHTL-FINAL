cmake_minimum_required(VERSION 3.20)
project(CHTL VERSION 0.1.0 LANGUAGES CXX)

# C++20标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 编译选项
if(MSVC)
    add_compile_options(/W4 /utf-8)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 包含目录
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third-party)

# 收集源文件
file(GLOB_RECURSE CHTL_SOURCES 
    "${CMAKE_SOURCE_DIR}/src/CHTL/**/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/Util/**/*.cpp"
)

file(GLOB_RECURSE CHTL_HEADERS 
    "${CMAKE_SOURCE_DIR}/src/CHTL/**/*.h"
    "${CMAKE_SOURCE_DIR}/src/CHTL/**/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/Util/**/*.h"
    "${CMAKE_SOURCE_DIR}/src/Util/**/*.hpp"
)

# CHTL编译器库
add_library(chtl_lib STATIC ${CHTL_SOURCES} ${CHTL_HEADERS})
target_include_directories(chtl_lib PUBLIC ${CMAKE_SOURCE_DIR}/src)

# CHTL编译器可执行文件
add_executable(chtl src/main.cpp)
target_link_libraries(chtl PRIVATE chtl_lib)

# 启用测试
enable_testing()

# 测试可执行文件
file(GLOB_RECURSE TEST_SOURCES "${CMAKE_SOURCE_DIR}/tests/*.cpp")
if(TEST_SOURCES)
    add_executable(chtl_tests ${TEST_SOURCES})
    target_link_libraries(chtl_tests PRIVATE chtl_lib)
    target_include_directories(chtl_tests PRIVATE ${CMAKE_SOURCE_DIR}/third-party)
    
    # 使用Catch2添加测试
    add_test(NAME chtl_tests COMMAND chtl_tests)
endif()

# 安装规则
install(TARGETS chtl DESTINATION bin)
install(TARGETS chtl_lib DESTINATION lib)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/CHTL/ 
        DESTINATION include/CHTL
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
