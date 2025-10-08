# Define the test executable
add_executable(RunTests
    TestStringUtil.cpp
    TestLexer.cpp
)

# Add include directories for third-party libraries like Catch2
target_include_directories(RunTests PRIVATE
    ${CMAKE_SOURCE_DIR}/third-party
)

# Link the test executable against our libraries and Catch2
target_link_libraries(RunTests PRIVATE
    UtilLib
    ChtlLib
)

# Add the test to CTest
add_test(NAME UnitTests COMMAND RunTests)