# CHTL 项目开发日志

## 项目概述

CHTL（CHTolly HyperText Language）是一个基于 C++20 的超文本语言编译器项目，采用 TDD（测试驱动开发）方法，使用 CMake 构建系统。

## 开发进度总览

```
[████████████████░░░░] 60% 完成

✅ Step 1: 项目基础设置
✅ Step 2: CHTL 特殊语法块支持
✅ Step 3: Parser 基础架构
⏳ Step 4: 代码生成器
⏳ Step 5: 完整编译流程
```

---

## Step 1: 项目基础设置

**日期**: 2025-10-14  
**状态**: ✅ 完成

### 成果

1. **项目结构**
   - CMake 构建系统（C++20）
   - 目录结构按 CHTL.md 规范组织
   - .gitignore 配置完善

2. **Python 构建脚本** (`build.py`)
   - 自动配置和构建
   - 运行测试
   - 查找和调用二进制文件
   - 跨平台支持（Windows/Linux/macOS）

3. **词法分析器 (Lexer)**
   - Token 类型定义
   - 基础 token 识别
   - 字符串、数字、标识符
   - 注释处理
   - 行列号跟踪

4. **TDD 测试**
   - Catch2 测试框架
   - 7 个测试用例
   - 63 个断言
   - 100% 通过

### 关键文件

- `CMakeLists.txt` - CMake 配置
- `build.py` - Python 构建脚本
- `src/CHTL/CHTLLexer/*` - 词法分析器
- `tests/test_lexer.cpp` - Lexer 测试
- `PROJECT_SETUP.md` - 项目设置文档
- `QUICKSTART.md` - 快速入门指南

---

## Step 2: CHTL 特殊语法块支持

**日期**: 2025-10-14  
**状态**: ✅ 完成

### 成果

1. **新增 Token 类型（30+ 个）**

   **特殊语法块**:
   - `[Template]`, `[Custom]`, `[Import]`, `[Origin]`
   - `[Namespace]`, `[Configuration]`, `[Info]`, `[Export]`

   **@ 前缀关键字**:
   - `@Style`, `@Element`, `@Var`, `@Html`
   - `@JavaScript`, `@Chtl`, `@Config`, `@CJmod`

   **其他关键字**:
   - `from`, `as`, `inherit`, `delete`, `insert`, `use`, `except`
   - `after`, `before`, `replace`

2. **增强的词法分析器**
   - `scanBracketKeyword()` - 识别 `[...]` 格式
   - 支持 `@` 前缀关键字
   - 扩展关键字映射

3. **TDD 测试**
   - 测试用例：7 → **11** (+4)
   - 断言数量：63 → **121** (+58)
   - 通过率：**100%**

### 关键文件

- `src/CHTL/CHTLLexer/Token.h` - 扩展的 Token 定义
- `src/CHTL/CHTLLexer/Lexer.cpp` - 增强的 Lexer 实现
- `tests/test_lexer.cpp` - 新增测试
- `examples/template_example.chtl` - 示例文件
- `STEP2_SUMMARY.md` - 详细总结

### TDD 流程

1. **红色阶段** ❌ - 编写测试，确认失败
2. **绿色阶段** ✅ - 实现功能，测试通过
3. **重构阶段** ♻️ - 优化代码，保持测试通过

---

## Step 3: Parser 基础架构

**日期**: 2025-10-14  
**状态**: ✅ 完成

### 成果

1. **AST 节点系统**
   - `BaseNode` - 抽象基类（使用访问者模式）
   - `ElementNode` - HTML 元素节点
   - `TextNode` - 文本节点
   - `NodeVisitor` - 访问者接口

2. **递归下降 Parser**
   - 元素解析
   - 属性解析（支持 `:` 和 `=`）
   - 文本块解析
   - 嵌套元素解析
   - 错误处理和恢复

3. **TDD 测试**
   - 测试用例：11 → **17** (+6)
   - 断言数量：121 → **181** (+60)
   - Parser 专项测试：6 个
   - 通过率：**100%**

4. **演示程序**
   - `parser_demo` - 完整的 Lexer + Parser 演示
   - AST 树状打印

### 关键文件

- `src/CHTL/CHTLNode/*` - AST 节点定义（7 个文件）
- `src/CHTL/CHTLParser/*` - Parser 实现（2 个文件）
- `tests/test_parser.cpp` - Parser 测试
- `src/demo_parser.cpp` - 演示程序
- `STEP3_SUMMARY.md` - 详细总结

### 技术亮点

**设计模式**:
- 访问者模式 - AST 遍历
- 递归下降解析
- 智能指针管理内存

**C++20 特性**:
- `std::unique_ptr`
- 结构化绑定
- `= default` / `= delete`
- `override` 关键字

### 示例输出

**输入 (CHTL)**:
```chtl
html {
    body {
        id: "main";
        div {
            text { "Welcome to CHTL" }
        }
    }
}
```

**输出 (AST)**:
```
<html>
  <body id="main">
    <div>
      "Welcome to CHTL"
    </div>
  </body>
</html>
```

---

## 技术栈

### 核心技术
- **语言**: C++20
- **构建系统**: CMake 3.20+
- **测试框架**: Catch2
- **开发方法**: TDD（测试驱动开发）

### 编译器要求
- GCC 10+ / Clang 12+ / MSVC 2019+
- 支持 C++20 标准

### 工具
- Python 3.6+ - 构建脚本
- Git - 版本控制

---

## 测试统计

### 总览

| Step | 测试用例 | 断言数量 | 通过率 |
|------|---------|---------|--------|
| Step 1 | 7 | 63 | 100% |
| Step 2 | 11 (+4) | 121 (+58) | 100% |
| Step 3 | 17 (+6) | 181 (+60) | 100% |

### 当前状态
```
===============================================================================
All tests passed (181 assertions in 17 test cases)
===============================================================================
```

---

## 代码统计

### 源代码

| 模块 | 文件数 | 代码行数 |
|------|--------|----------|
| Lexer | 4 个 | ~500 行 |
| Parser | 2 个 | ~300 行 |
| AST 节点 | 7 个 | ~200 行 |
| 测试 | 3 个 | ~550 行 |
| 演示程序 | 2 个 | ~200 行 |
| **总计** | **18 个** | **~1750 行** |

### 二进制文件

| 文件 | 大小 | 说明 |
|------|------|------|
| `chtl` | 207.2 KB | CHTL 编译器 |
| `chtl_tests` | 7098.2 KB | 测试程序 |
| `parser_demo` | 603.4 KB | Parser 演示 |

---

## 下一步计划

### Step 4: 代码生成器 (Generator)

**目标**: 从 AST 生成 HTML 输出

#### 计划功能
1. **HTML Generator**
   - 从 AST 生成 HTML
   - 属性处理
   - 文本内容输出
   - 格式化输出

2. **样式处理**
   - 内联样式支持
   - style 块处理
   - CSS 生成

3. **完整编译流程**
   - Lexer → Parser → Generator
   - 文件输出
   - 错误报告

#### TDD 计划
1. 编写 Generator 测试
2. 实现基础 HTML 生成
3. 添加样式支持
4. 完整集成测试

---

## 项目文件结构

```
CHTL/
├── CMakeLists.txt              # CMake 主配置
├── build.py                    # Python 构建脚本
├── .gitignore                  # Git 版本控制配置
│
├── src/                        # 源代码
│   ├── CMakeLists.txt
│   ├── main.cpp               # 主程序
│   ├── demo_parser.cpp        # Parser 演示
│   │
│   ├── CHTL/                  # CHTL 核心模块
│   │   ├── CHTLLexer/        # 词法分析器 ✅
│   │   │   ├── Lexer.h/cpp
│   │   │   └── Token.h/cpp
│   │   │
│   │   ├── CHTLParser/       # 语法分析器 ✅
│   │   │   └── Parser.h/cpp
│   │   │
│   │   ├── CHTLNode/         # AST 节点 ✅
│   │   │   ├── BaseNode.h/cpp
│   │   │   ├── ElementNode.h/cpp
│   │   │   ├── TextNode.h/cpp
│   │   │   └── NodeVisitor.h
│   │   │
│   │   ├── CHTLContext/      # 上下文 ⏳
│   │   └── CHTLGenerator/    # 代码生成器 ⏳
│   │
│   └── Util/                  # 工具类
│
├── tests/                     # 测试 ✅
│   ├── CMakeLists.txt
│   ├── test_main.cpp
│   ├── test_lexer.cpp        # Lexer 测试
│   └── test_parser.cpp       # Parser 测试
│
├── third-party/              # 第三方库
│   └── catch.hpp             # Catch2 测试框架
│
├── examples/                 # 示例文件
│   ├── simple.chtl
│   ├── template_example.chtl
│   └── parser_demo.cpp
│
└── docs/                     # 文档
    ├── CHTL.md               # 语法规范
    ├── PROJECT_SETUP.md      # 项目设置
    ├── QUICKSTART.md         # 快速入门
    ├── STEP2_SUMMARY.md      # Step 2 总结
    ├── STEP3_SUMMARY.md      # Step 3 总结
    └── DEVELOPMENT_LOG.md    # 开发日志（本文件）
```

---

## 最佳实践

### TDD 流程
1. **红色** ❌ - 编写失败的测试
2. **绿色** ✅ - 最小化实现让测试通过
3. **重构** ♻️ - 优化代码，保持测试通过

### 代码质量
- 编译器警告级别: `-Wall -Wextra -Wpedantic -Werror`
- 零警告通过 ✓
- 代码格式规范 ✓
- 100% 测试通过率 ✓

### 提交规范
- 小步迭代
- 频繁提交
- 清晰的提交信息
- 每个功能独立提交

---

## 参考资源

- **CHTL 语法规范**: `CHTL.md`
- **项目设置**: `PROJECT_SETUP.md`
- **快速入门**: `QUICKSTART.md`
- **Catch2**: https://github.com/catchorg/Catch2
- **C++ Core Guidelines**: https://isocpp.github.io/CppCoreGuidelines/
- **Crafting Interpreters**: https://craftinginterpreters.com/

---

## 许可证

MIT License

---

**最后更新**: 2025-10-14  
**当前版本**: v0.1.0-alpha  
**项目状态**: 🚀 积极开发中
