# CHTL/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

project(CHTL LANGUAGES CXX)

# 设置C++标准为20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 使用 glob 自动查找所有源文件
file(GLOB_RECURSE CHTL_SOURCES
    "src/*.cpp"
    "src/bridge/*.cpp"
    "src/eval/*.cpp"
    "src/generator/*.cpp"
    "src/lexer/*.cpp"
    "src/loader/*.cpp"
    "src/nodes/*.cpp"
    "src/parser/*.cpp"
    "CHTLJS/src/*.cpp"
    "CHTLJS/src/generator/*.cpp"
    "CHTLJS/src/lexer/*.cpp"
    "CHTLJS/src/nodes/*.cpp"
    "CHTLJS/src/parser/*.cpp"
    src/CMOD/packer.cpp
    src/CJMOD/loader.cpp
)

# HEADERS 变量现在是不必要的，因为 target_include_directories 已经处理了路径。
# 我们可以将其保留为空，或者完全删除它。为了清晰起见，我们将其删除。

# 添加库
add_library(CHTL STATIC ${CHTL_SOURCES})

# 公开include目录
target_include_directories(CHTL PUBLIC
    .
    include
    include/bridge
    include/eval
    include/generator
    CHTLJS/include/generator
    include/lexer
    include/loader
    include/nodes
    include/parser
)

# 添加测试子目录
add_subdirectory(tests)

# 添加CLI可执行文件
add_executable(chtl cli/main.cpp)
target_link_libraries(chtl PRIVATE CHTL)

# 添加语言服务器可执行文件
add_executable(chtl-ls langserver/src/main.cpp)
target_link_libraries(chtl-ls PRIVATE CHTL)
target_include_directories(chtl-ls PRIVATE ../third-party)