add_library(chtl_lib STATIC
    CHTLLexer/Lexer.cpp
)

target_include_directories(chtl_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(chtl main.cpp)
target_link_libraries(chtl PRIVATE chtl_lib)