cmake_minimum_required(VERSION 3.20)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# 设置C++20标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置编译选项
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 包含第三方库
include_directories(third-party)

# 添加Catch2测试框架
add_subdirectory(third-party/catch2)

# 创建CHTL核心库
add_library(chtl_core
    src/CHTL/CHTLLexer/CHTLLexer.cpp
    src/CHTL/CHTLLexer/Token.cpp
    src/CHTL/CHTLLexer/GlobalMap.cpp
    src/CHTL/CHTLParser/CHTLParser.cpp
    src/CHTL/CHTLGenerator/CHTLGenerator.cpp
    src/CHTL/CHTLNode/BaseNode.cpp
    src/CHTL/CHTLNode/ElementNode.cpp
    src/CHTL/CHTLNode/TextNode.cpp
    src/CHTL/CHTLNode/CommentNode.cpp
    src/CHTL/CHTLNode/StyleNode.cpp
    src/CHTL/CHTLNode/ScriptNode.cpp
    src/CHTL/CHTLNode/TemplateNode.cpp
    src/CHTL/CHTLNode/CustomNode.cpp
    src/CHTL/CHTLNode/OriginNode.cpp
    src/CHTL/CHTLNode/ImportNode.cpp
    src/CHTL/CHTLNode/ConfigNode.cpp
    src/CHTL/CHTLNode/NamespaceNode.cpp
    src/CHTL/CHTLNode/OperatorNode.cpp
    src/CHTL/CHTLContext/CHTLContext.cpp
    src/CHTL/CHTLLoader/CHTLLoader.cpp
    src/CHTL/CHTLManage/CHTLManage.cpp
    src/CHTL/CHTLState/CHTLState.cpp
    src/CHTL/CHTLStrategy/CHTLStrategy.cpp
    src/CHTL/CHTLIOStream/CHTLIOStream.cpp
    src/CHTL/CMODSystem/CMODSystem.cpp
    src/Util/FileSystem/FileSystem.cpp
    src/Util/StringUtil/StringUtil.cpp
    src/Util/ZipUtil/ZipUtil.cpp
    src/SharedCore/SharedCore.cpp
)

# 设置头文件目录
target_include_directories(chtl_core PUBLIC
    src/CHTL
    src/Util
    src/SharedCore
    third-party
)

# 创建CHTL编译器可执行文件
add_executable(chtl_compiler
    src/main.cpp
)

target_link_libraries(chtl_compiler chtl_core)

# 创建测试可执行文件
add_executable(chtl_tests
    tests/main.cpp
    tests/UtilTest/UtilTest.cpp
    tests/TokenTest/TokenTest.cpp
    tests/TokenTest/TokenPrint.cpp
    tests/TokenTest/TokenTable.cpp
    tests/AstTest/AstTest.cpp
    tests/AstTest/ASTPrint.cpp
    tests/AstTest/ASTGraph.cpp
    tests/LexerTest/LexerTest.cpp
    tests/ParserTest/ParserTest.cpp
    tests/GeneratorTest/GeneratorTest.cpp
)

target_link_libraries(chtl_tests chtl_core Catch2::Catch2WithMain)

# 启用测试
enable_testing()
add_test(NAME chtl_tests COMMAND chtl_tests)

# 创建CHTL JS核心库
add_library(chtl_js_core
    src/CHTLJS/CHTLJSLexer/CHTLJSLexer.cpp
    src/CHTLJS/CHTLJSLexer/Token.cpp
    src/CHTLJS/CHTLJSLexer/GlobalMap.cpp
    src/CHTLJS/CHTLJSParser/CHTLJSParser.cpp
    src/CHTLJS/CHTLJSGenerator/CHTLJSGenerator.cpp
    src/CHTLJS/CHTLJSNode/CHTLJSBaseNode.cpp
    src/CHTLJS/CHTLJSContext/CHTLJSContext.cpp
    src/CHTLJS/CHTLJSLoader/CHTLJSLoader.cpp
    src/CHTLJS/CHTLJSManage/CHTLJSManage.cpp
    src/CHTLJS/CHTLJSState/CHTLJSState.cpp
    src/CHTLJS/CHTLJSStrategy/CHTLJSStrategy.cpp
    src/CHTLJS/CHTLJSIOStream/CHTLJSIOStream.cpp
    src/CHTLJS/CJMODSystem/CJMODSystem.cpp
)

target_include_directories(chtl_js_core PUBLIC
    src/CHTLJS
    src/Util
    src/SharedCore
    third-party
)

# 创建CHTL JS编译器可执行文件
add_executable(chtl_js_compiler
    src/chtl_js_main.cpp
)

target_link_libraries(chtl_js_compiler chtl_js_core chtl_core)

# 创建模块目录
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/module)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/module/cmod)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/module/cjmod)