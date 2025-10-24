cmake_minimum_required(VERSION 3.20)

project(CHTL CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect source files
set(CHTL_SOURCES
    CHTL/main.cpp
    CHTL/CHTL/CHTLLexer/Lexer.cpp
    CHTL/CHTL/CHTLParser/Parser.cpp
    CHTL/CHTL/CHTLGenerator/HTMLGenerator.cpp
)

# Main executable
add_executable(chtl ${CHTL_SOURCES})
target_include_directories(chtl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Google Test integration
enable_testing()

add_subdirectory(third-party/googletest)

# Collect test files
set(TEST_SOURCES
    CHTL/Test/sample_test.cpp
    CHTL/Test/LexerTest.cpp
    CHTL/Test/ParserTest.cpp
    # Add new test files here
)

set(COMPILER_LIB_SOURCES
    CHTL/CHTL/CHTLLexer/Lexer.cpp
    CHTL/CHTL/CHTLParser/Parser.cpp
    CHTL/CHTL/CHTLGenerator/HTMLGenerator.cpp
)


add_library(compiler_lib ${COMPILER_LIB_SOURCES})
target_include_directories(compiler_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


add_executable(run_tests ${TEST_SOURCES})
target_link_libraries(run_tests
  PRIVATE
  gtest_main
  compiler_lib
)
target_include_directories(run_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


include(GoogleTest)
gtest_discover_tests(run_tests)
