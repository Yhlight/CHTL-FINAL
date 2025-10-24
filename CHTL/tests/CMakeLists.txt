# CHTL/tests/CMakeLists.txt

# Common setup for all tests
macro(add_chkl_test test_name)
    add_executable(${test_name} ${ARGN})
    target_link_libraries(${test_name} PRIVATE CHTL)
    target_include_directories(${test_name} PRIVATE ../../third-party)
    add_test(NAME ${test_name} COMMAND ${test_name})
endmacro()

# Add test executables
add_chkl_test(TokenTest TokenTest.cpp)
add_chkl_test(AstTest AstTest.cpp)
add_chkl_test(GeneratorTest GeneratorTest.cpp)
add_chkl_test(EvaluatorTest EvaluatorTest.cpp)
add_chkl_test(LexerTest LexerTest.cpp)
add_chkl_test(ParserTest ParserTest.cpp)
add_chkl_test(ArithmeticTest ArithmeticTest.cpp)
add_chkl_test(AttributeAccessTest AttributeAccessTest.cpp)
add_chkl_test(CustomizationTest CustomizationTest.cpp)

# CHTL JS Tests
add_chkl_test(CHTLJSPreprocessorTest CHTLJSTests/PreprocessorTests.cpp)
add_chkl_test(CHTLJSLexerTest CHTLJSLexerTest.cpp)
add_chkl_test(CHTLJSParserTest CHTLJSParserTest.cpp)
add_chkl_test(IfElseGeneratorTest IfElseGeneratorTest.cpp)
add_chkl_test(BridgeTest BridgeTest.cpp)
add_chkl_test(CHTLJSGeneratorTest CHTLJS/GeneratorTest.cpp)
add_chkl_test(SerializationTest SerializationTest.cpp)
add_chkl_test(ModuleTest ModuleTest.cpp)
add_chkl_test(CJMODAPITest CJMODAPITest.cpp)
add_chkl_test(ValidatorTest ValidatorTest.cpp)

# Add extra include directories for JS tests
target_include_directories(CHTLJSPreprocessorTest PRIVATE ../CHTLJS/include)
target_include_directories(CHTLJSGeneratorTest PRIVATE ../CHTLJS/include)
target_include_directories(SerializationTest PRIVATE ../CHTLJS/include)
target_include_directories(ModuleTest PRIVATE ../CHTLJS/include)
target_include_directories(CJMODAPITest PRIVATE ../CHTLJS/include)
