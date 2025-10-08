include(GoogleTest)

add_executable(unit_tests sample_test.cpp lexer_test.cpp)

target_link_libraries(unit_tests PRIVATE gtest_main chtl_lib)

gtest_discover_tests(unit_tests)