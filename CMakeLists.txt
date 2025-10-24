cmake_minimum_required(VERSION 3.20)

project(CHTL CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Main executable
add_executable(chtl CHTL/main.cpp)
target_include_directories(chtl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Google Test integration
enable_testing()

add_subdirectory(third-party/googletest)

add_executable(run_tests CHTL/Test/sample_test.cpp)

target_link_libraries(run_tests
  PRIVATE
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(run_tests)
