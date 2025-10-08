set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include the necessary directories
include_directories(../third-party)
include_directories(../) # Include the root to find CHTL/

# Add the test executable with all required source files
add_executable(chttl_tests
    TestMain.cpp
    Lexer.test.cpp
    Parser.test.cpp
    Generator.test.cpp
    ../CHTL/CHTLLexer/Lexer.cpp
    ../CHTL/CHTLParser/Parser.cpp
    ../CHTL/CHTLGenerator/Generator.cpp
)

# Add a test to CTest
add_test(NAME chttl_core_tests COMMAND chttl_tests)