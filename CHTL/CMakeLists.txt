# Create a static library for the CHTL compiler components
add_library(ChtlLib STATIC
    CHTLLexer/Token.cpp
    CHTLLexer/Lexer.cpp
    CHTLParser/Parser.cpp
)

# Publicly expose the header files of the ChtlLib library
target_include_directories(ChtlLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link against the utility library
target_link_libraries(ChtlLib PUBLIC UtilLib)