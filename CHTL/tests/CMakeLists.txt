# CHTL/tests/CMakeLists.txt

# Create a library for the main function of Catch2
add_library(CatchMain TestMain.cpp)
target_include_directories(CatchMain PRIVATE ../../third-party)

# 添加测试可执行文件
add_executable(TokenTest TokenTest.cpp)
add_executable(AstTest AstTest.cpp)
add_executable(GeneratorTest GeneratorTest.cpp)
add_executable(EvaluatorTest EvaluatorTest.cpp)
add_executable(LexerTest LexerTest.cpp)
add_executable(ParserTest ParserTest.cpp)

# 链接CHTL库
target_link_libraries(TokenTest PRIVATE CHTL CatchMain)
target_link_libraries(AstTest PRIVATE CHTL CatchMain)
target_link_libraries(GeneratorTest PRIVATE CHTL CatchMain)
target_link_libraries(EvaluatorTest PRIVATE CHTL CatchMain)
target_link_libraries(LexerTest PRIVATE CHTL CatchMain)
target_link_libraries(ParserTest PRIVATE CHTL CatchMain)

# 添加third-party目录以便找到catch.hpp
target_include_directories(TokenTest PRIVATE ../../third-party)
target_include_directories(AstTest PRIVATE ../../third-party)
target_include_directories(GeneratorTest PRIVATE ../../third-party)
target_include_directories(EvaluatorTest PRIVATE ../../third-party)
target_include_directories(LexerTest PRIVATE ../../third-party)
target_include_directories(ParserTest PRIVATE ../../third-party)

# CHTL JS Tests
add_executable(CHTLJSLexerTest CHTLJSLexerTest.cpp)
target_link_libraries(CHTLJSLexerTest PRIVATE CHTL CatchMain)
target_include_directories(CHTLJSLexerTest PRIVATE ../../third-party)

add_executable(CHTLJSParserTest CHTLJSParserTest.cpp)
target_link_libraries(CHTLJSParserTest PRIVATE CHTL CatchMain)
target_include_directories(CHTLJSParserTest PRIVATE ../../third-party)

add_executable(IfElseGeneratorTest IfElseGeneratorTest.cpp)
target_link_libraries(IfElseGeneratorTest PRIVATE CHTL CatchMain)
target_include_directories(IfElseGeneratorTest PRIVATE ../../third-party)

add_executable(ConfigScannerTest ConfigScannerTest.cpp)
target_link_libraries(ConfigScannerTest PRIVATE CHTL CatchMain)
target_include_directories(ConfigScannerTest PRIVATE ../../third-party)

add_executable(IfStatementTest src/parser/IfStatementTest.cpp)
target_link_libraries(IfStatementTest PRIVATE CHTL CatchMain)
target_include_directories(IfStatementTest PRIVATE ../../third-party)
