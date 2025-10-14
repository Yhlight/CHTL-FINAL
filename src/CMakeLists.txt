# CHTL 编译器核心库
add_library(chtl_core STATIC
    # Lexer 词法分析器
    CHTL/CHTLLexer/Lexer.cpp
    CHTL/CHTLLexer/Token.cpp
    
    # Parser 语法分析器
    CHTL/CHTLParser/Parser.cpp
    
    # Node AST 节点
    CHTL/CHTLNode/BaseNode.cpp
    CHTL/CHTLNode/ElementNode.cpp
    CHTL/CHTLNode/TextNode.cpp
    CHTL/CHTLNode/StyleNode.cpp
    CHTL/CHTLNode/ScriptNode.cpp
    
    # Context 上下文
    # CHTL/CHTLContext/Context.cpp
    
    # Generator 生成器
    CHTL/CHTLGenerator/Generator.cpp
)

target_include_directories(chtl_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# CHTL 编译器可执行文件
add_executable(chtl
    main.cpp
)

target_link_libraries(chtl PRIVATE chtl_core)

# Parser 演示程序
add_executable(parser_demo
    demo_parser.cpp
)

target_link_libraries(parser_demo PRIVATE chtl_core)
