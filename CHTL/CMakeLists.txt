cmake_minimum_required(VERSION 3.10)
project(CHTL_Compiler)

set(CMAKE_CXX_STANDARD 20)

# Define the lexer library
add_library(CHTLLexer STATIC
    CHTL/CHTLLexer/Lexer.cpp
)

# Define the parser library
add_library(CHTLParser STATIC
    CHTL/CHTLParser/Parser.cpp
)

# Define the TokenTest executable
add_executable(TokenTest
    Test/TokenTest/main.cpp
)

# Link the TokenTest executable against the lexer library
target_link_libraries(TokenTest CHTLLexer)

# Define the AstTest executable
add_executable(AstTest
    Test/AstTest/main.cpp
)

# Link the AstTest executable against the parser and lexer libraries
target_link_libraries(AstTest CHTLGenerator CHTLParser CHTLLexer CHTLEvaluator CHTLAnalyzer CHTLNode)

# Define the generator library
add_library(CHTLGenerator STATIC
    CHTL/CHTLGenerator/Generator.cpp
)

# Define the evaluator library
add_library(CHTLEvaluator STATIC
    CHTL/CHTLEvaluator/Evaluator.cpp
)

# Define the analyzer library
add_library(CHTLAnalyzer STATIC
    CHTL/CHTLAnalyzer/DocumentMap.cpp
)

# Define the Node library
add_library(CHTLNode STATIC
    CHTL/CHTLNode/Style.cpp
    CHTL/CHTLNode/Template.cpp
)

# Define the GeneratorTest executable
add_executable(GeneratorTest
    Test/GeneratorTest/main.cpp
)

# Link the GeneratorTest executable against the generator, parser, and lexer libraries
target_link_libraries(GeneratorTest CHTLGenerator CHTLParser CHTLLexer CHTLEvaluator CHTLAnalyzer CHTLNode)

# Define the CHTL executable
add_executable(CHTL
    main.cpp
)

# Link the CHTL executable against all the libraries
target_link_libraries(CHTL CHTLGenerator CHTLParser CHTLLexer CHTLEvaluator CHTLAnalyzer CHTLNode)
