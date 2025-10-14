# CHTL 项目设置说明

## 项目概述

CHTL（CHTolly HyperText Language）是一个基于 C++20 实现的超文本语言编译器项目。

## 项目结构

```
CHTL/
├── src/                      # 源代码目录
│   ├── CHTL/                 # CHTL 编译器核心
│   │   ├── CHTLLexer/       # 词法分析器
│   │   ├── CHTLParser/      # 语法分析器（待实现）
│   │   ├── CHTLNode/        # AST 节点（待实现）
│   │   ├── CHTLContext/     # 上下文（待实现）
│   │   └── CHTLGenerator/   # 代码生成器（待实现）
│   └── main.cpp             # 主程序入口
├── tests/                   # 测试目录
│   ├── test_main.cpp        # 测试主文件
│   └── test_lexer.cpp       # 词法分析器测试
├── third-party/             # 第三方库
│   └── catch.hpp            # Catch2 测试框架
├── examples/                # 示例文件
│   └── simple.chtl          # 简单示例
├── CMakeLists.txt           # CMake 配置
├── build.py                 # Python 构建脚本
├── .gitignore               # Git 忽略文件
└── CHTL.md                  # CHTL 语法规范文档
```

## 依赖要求

- **C++ 编译器**: 支持 C++20 标准
  - GCC 10+ / Clang 12+ / MSVC 2019+
- **CMake**: 3.20 或更高版本
- **Python**: 3.6+ (用于构建脚本)

## 构建方式

### 方式一：使用 Python 构建脚本（推荐）

#### 1. 完整构建流程
```bash
python build.py --all
```

这将执行：
- 清理构建目录
- 配置 CMake 项目
- 编译源代码
- 运行测试
- 列出生成的二进制文件

#### 2. 分步构建

```bash
# 清理构建目录
python build.py --clean

# 配置项目
python build.py --configure

# 构建项目
python build.py --build

# 运行测试
python build.py --test

# 列出生成的二进制文件
python build.py --list
```

#### 3. 运行 CHTL 编译器

```bash
# 编译 CHTL 文件
python build.py --run examples/simple.chtl

# 仅输出 token 列表
python build.py --run examples/simple.chtl --tokens
```

#### 4. Release 构建

```bash
python build.py --all --build-type Release
```

### 方式二：使用 CMake 直接构建

```bash
# 创建构建目录
mkdir build
cd build

# 配置
cmake -DCMAKE_BUILD_TYPE=Debug ..

# 构建
cmake --build .

# 运行测试
./bin/chtl_tests

# 运行编译器
./bin/chtl ../examples/simple.chtl
```

## TDD 开发流程

本项目采用 TDD (Test-Driven Development) 方式开发：

1. **编写测试**: 在 `tests/` 目录下创建测试文件
2. **运行测试**: 使用 `python build.py --test` 确认测试失败
3. **实现功能**: 在 `src/` 目录下实现对应功能
4. **运行测试**: 确认测试通过
5. **重构代码**: 优化代码质量

### 测试框架

使用 Catch2 单头文件版本，位于 `third-party/catch.hpp`。

### 测试示例

```cpp
#include "catch.hpp"
#include "CHTL/CHTLLexer/Lexer.h"

TEST_CASE("描述测试内容", "[tag]") {
    SECTION("具体测试场景") {
        // Arrange (准备)
        CHTL::Lexer lexer("input");
        
        // Act (执行)
        auto result = lexer.nextToken();
        
        // Assert (断言)
        REQUIRE(result.type == CHTL::TokenType::IDENTIFIER);
    }
}
```

## 构建产物

构建产物位于 `build/` 目录：

```
build/
├── bin/              # 可执行文件
│   ├── chtl          # CHTL 编译器
│   └── chtl_tests    # 测试程序
└── lib/              # 静态库
    └── libchtl_core.a
```

## Python 构建脚本功能

`build.py` 提供以下功能：

1. **自动检测操作系统**: 自动适配 Windows/Linux/macOS
2. **查找二进制文件**: 自动定位编译产物
3. **多核编译**: Linux/macOS 下自动使用多核编译
4. **友好的输出**: 使用 emoji 和颜色标识不同阶段

## 开发建议

1. **遵循 C++20 标准**: 充分利用现代 C++ 特性
2. **先写测试**: TDD 方式确保代码质量
3. **小步迭代**: 每次实现一个小功能，确保测试通过
4. **保持简洁**: 避免过度设计，逐步完善

## 当前进度

- ✅ 项目基础结构
- ✅ CMake 构建系统
- ✅ Python 构建脚本
- ✅ 词法分析器基础实现
- ✅ 词法分析器测试
- ⏳ 语法分析器
- ⏳ AST 节点定义
- ⏳ 代码生成器

## 下一步计划

1. 完善词法分析器，支持更多 CHTL 语法元素
2. 实现基本的语法分析器
3. 定义 AST 节点结构
4. 实现简单的 HTML 生成器

## 故障排除

### CMake 未找到
确保 CMake 已安装并添加到 PATH：
```bash
cmake --version
```

### 编译错误
检查编译器是否支持 C++20：
```bash
g++ --version  # GCC 10+
clang++ --version  # Clang 12+
```

### 测试失败
运行单个测试获取详细信息：
```bash
./build/bin/chtl_tests -s
```

## 许可证

MIT License - 详见 LICENSE 文件
