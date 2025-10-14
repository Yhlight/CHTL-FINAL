# CHTL编译器项目

CHTL（超文本语言）是基于C++20实现的新一代超文本语言编译器，提供更符合开发者习惯的HTML/CSS代码编写方式。

## 项目状态

当前版本：**v0.1.0-dev** （开发中）

已完成功能：
- ✅ C++20项目基础架构
- ✅ CMake构建系统
- ✅ Python自动化构建脚本
- ✅ Token基础结构
- ✅ 单元测试框架（Catch2）
- ✅ TDD开发流程

正在开发：
- 🔨 词法分析器（Lexer）
- 🔨 语法分析器（Parser）
- 🔨 AST节点系统

## 项目结构

```
CHTL/
├── src/                    # 源代码目录
│   ├── CHTL/              # CHTL编译器核心
│   │   ├── CHTLLexer/     # 词法分析器
│   │   │   ├── Token.h/cpp        # Token定义
│   │   │   ├── Lexer.h/cpp        # 词法分析器（待实现）
│   │   │   └── GlobalMap.h/cpp    # 全局映射表（待实现）
│   │   ├── CHTLParser/    # 语法分析器（待实现）
│   │   ├── CHTLNode/      # AST节点（待实现）
│   │   ├── CHTLContext/   # 上下文管理（待实现）
│   │   ├── CHTLGenerator/ # 代码生成器（待实现）
│   │   ├── CHTLState/     # 状态机（待实现）
│   │   └── CHTLStrategy/  # 策略模式（待实现）
│   ├── Util/              # 工具类
│   └── main.cpp           # 主程序入口
├── tests/                 # 测试目录
│   └── test_token.cpp     # Token测试
├── third-party/           # 第三方依赖
│   ├── catch.hpp          # Catch2测试框架
│   ├── miniz.h/c          # ZIP压缩库
│   └── ...
├── build.py               # Python构建脚本
├── CMakeLists.txt         # CMake配置
├── .gitignore            # Git忽略配置
├── CHTL.md               # CHTL语言规范文档
└── README_ZH.md          # 项目说明（本文件）
```

## 构建要求

- **C++编译器**：支持C++20标准（GCC 10+, Clang 10+, MSVC 2019+）
- **CMake**：3.20或更高版本
- **Python**：3.6或更高版本（用于构建脚本）

## 快速开始

### 1. 配置和构建

使用Python构建脚本（推荐）：

```bash
# 配置、构建并运行测试
python3 build.py configure build test

# 或者一次性执行所有步骤
python3 build.py all

# 清理后重新构建
python3 build.py clean build
```

使用CMake（传统方式）：

```bash
# 配置
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

# 构建
cmake --build build -j

# 运行测试
./build/bin/chtl_tests
```

### 2. 运行编译器

```bash
# 使用Python构建脚本
python3 build.py run -- example.chtl

# 或直接运行
./build/bin/chtl example.chtl
```

### 3. 运行测试

```bash
# 使用Python构建脚本
python3 build.py test

# 或直接运行测试可执行文件
./build/bin/chtl_tests
```

## 构建脚本使用

`build.py` 提供了以下命令：

- `configure` - 配置CMake项目
- `build` - 构建项目
- `test` - 运行测试
- `run` - 运行CHTL编译器
- `clean` - 清理构建目录
- `all` - 执行 configure + build + test

**示例：**

```bash
# 配置并构建Release版本
python3 build.py --build-type Release configure build

# 指定自定义构建目录
python3 build.py --build-dir my_build configure build

# 运行编译器并传递参数
python3 build.py run -- input.chtl --output output.html
```

## 开发指南

### TDD开发流程

本项目采用测试驱动开发（TDD）方法：

1. **编写测试** - 先编写测试用例
2. **运行测试** - 确保测试失败（红灯）
3. **实现功能** - 编写最小可行代码
4. **运行测试** - 确保测试通过（绿灯）
5. **重构代码** - 优化代码结构
6. **重复** - 继续下一个功能

### 添加新功能

1. 在 `tests/` 目录创建测试文件
2. 编写失败的测试用例
3. 在 `src/CHTL/` 对应模块实现功能
4. 运行测试确保通过
5. 提交代码

### 代码规范

- 使用C++20标准特性
- 遵循RAII原则
- 优先使用智能指针
- 清晰的命名约定
- 充分的注释和文档

## 版本控制

项目使用Git进行版本控制，`.gitignore`已配置排除：
- CMake构建产物（`build/`目录）
- 编译生成的二进制文件
- IDE配置文件
- 临时文件

## 测试覆盖

当前测试覆盖：
- ✅ Token基础功能
- ✅ Token类型判断
- ✅ SourceLocation
- ⏳ Lexer词法分析
- ⏳ Parser语法分析

运行测试查看详细结果：
```bash
python3 build.py test
```

## 贡献指南

1. Fork本项目
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启Pull Request

## 许可证

本项目采用MIT开源协议。详见 [LICENSE](LICENSE) 文件。

## 参考文档

- [CHTL语言规范](CHTL.md) - 完整的CHTL语言语法规范
- [项目结构建议](CHTL.md#项目结构建议) - CHTL.md中推荐的项目组织方式

## 联系方式

如有问题或建议，欢迎提交Issue。

---

**注意**：本项目正处于早期开发阶段，API和功能可能会有较大变化。
