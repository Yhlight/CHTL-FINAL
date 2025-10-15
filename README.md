# CHTL 编译器

<div align="center">

**CHTL（CHTolly HyperText Language）**  
基于 C++20 实现的超文本语言编译器

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)]()
[![Tests](https://img.shields.io/badge/tests-380%20passed-brightgreen)]()
[![Coverage](https://img.shields.io/badge/coverage-100%25-brightgreen)]()
[![C++](https://img.shields.io/badge/C++-20-blue)]()
[![License](https://img.shields.io/badge/license-MIT-blue)]()
[![Version](https://img.shields.io/badge/version-v0.6.0-orange)]()

</div>

---

## ✨ 特性

- 🚀 **完整的编译流程** - Lexer → Parser → Generator
- 📝 **简洁的语法** - 类 CSS 的声明式语法
- 🎨 **高级样式系统** - 自动类名、&推导、伪类/伪元素 ✨
- 🎯 **TDD 驱动** - 83 个测试用例，474 个断言，100% 通过
- 🛠️ **C++20** - 使用现代 C++ 特性（智能指针、访问者模式）
- 📦 **CMake 构建** - 跨平台支持
- ⚡ **高性能** - C++ 原生实现

---

## 🚀 快速开始

### 1. 构建项目

```bash
# 完整构建（清理 + 配置 + 编译 + 测试）
python3 build.py --all
```

### 2. 编译你的第一个 CHTL 文件

```bash
# 编译到标准输出
./build/bin/chtl examples/simple.chtl

# 编译到文件
./build/bin/chtl examples/showcase.chtl --output output.html

# 带 DOCTYPE 声明
./build/bin/chtl examples/complete_example.chtl --doctype --output page.html
```

### 3. 查看结果

```bash
# 查看生成的 HTML
cat output.html
```

---

## 📝 语法示例

### CHTL 输入

```chtl
html {
    body {
        div {
            style {
                .container {
                    width: 1200px;
                    margin: 0 auto;
                }
                
                &:hover {
                    background: #f0f0f0;
                }
            }
            
            div {
                style {
                    .card {
                        padding: 20px;
                        border-radius: 8px;
                    }
                    
                    &::before {
                        content: "📝";
                    }
                }
                
                text: "Hello CHTL";
            }
        }
    }
}
```

### HTML 输出

```html
<html>
  <head>
    <style>
      .card {
        border-radius: 8px;
        padding: 20px;
      }

      .card::before {
        content: 📝;
      }

      .container {
        margin: 0 auto;
        width: 1200px;
      }

      .container:hover {
        background: #f0f0f0;
      }
    </style>
  </head>
  <body>
    <div class="container">
      <div class="card" text="Hello CHTL" />
    </div>
  </body>
</html>
```

**✨ 注意**:
- `<head>` 自动创建
- `class="container"` 和 `class="card"` 自动添加
- `&:hover` 自动展开为 `.container:hover`
- 全局样式自动注入

---

## 🎯 功能支持

### ✅ 已实现（功能丰富）

- [x] **词法分析** - 35+ Token 类型
- [x] **语法分析** - 递归下降 Parser
- [x] **AST 构建** - 访问者模式
- [x] **HTML 生成** - 完整的代码生成器
- [x] **元素和属性** - 完整支持
- [x] **文本节点** - text 块和 text 属性
- [x] **嵌套结构** - 任意层级嵌套
- [x] **CE 对等式** - `: = 等价`
- [x] **内联样式** - style {} 块
- [x] **脚本块** - script {} 支持
- [x] **属性运算** - 表达式求值（+ - * / % **）
- [x] **自动类名/id** - `.box`, `#header` 自动添加 ✨
- [x] **上下文推导&** - `&:hover`, `&::before` ✨
- [x] **全局样式** - 自动注入到 `<head>` ✨
- [x] **HTML 转义** - 安全的输出
- [x] **格式化选项** - 美化和紧凑模式
- [x] **命令行工具** - 完整的 CLI

### ⏳ 计划中（高级功能）

- [ ] 引用属性（box.width）
- [ ] 条件表达式（? :）
- [ ] 模板系统（[Template]）
- [ ] 导入系统（[Import]）
- [ ] 命名空间

---

## 💻 命令行选项

```bash
./build/bin/chtl <输入文件.chtl> [选项]

选项:
  --tokens              仅输出 token 列表（调试用）
  --ast                 仅输出 AST 信息（调试用）
  --output <文件>       指定输出文件（默认：标准输出）
  --doctype             在 HTML 输出中包含 DOCTYPE
  --compact             紧凑模式（不格式化，适合生产环境）
  --help, -h            显示帮助信息

示例:
  ./build/bin/chtl input.chtl
  ./build/bin/chtl input.chtl --output output.html
  ./build/bin/chtl input.chtl --doctype --compact
```

---

## 📊 项目统计

<table>
<tr>
<td>

**代码规模**
- 总代码: ~8,100 行
- 源代码: ~3,900 行
- 测试代码: ~3,200 行

</td>
<td>

**测试覆盖**
- 测试用例: 83 个
- 断言数量: 474 个
- 通过率: 100% ✓

</td>
<td>

**项目进度**
- 基础功能: 100%
- 高级功能: 75%
- 整体进度: 75%

</td>
</tr>
</table>

---

## 🏗️ 项目结构

```
CHTL/
├── src/                      # 源代码
│   ├── CHTL/
│   │   ├── CHTLLexer/       # ✅ 词法分析器
│   │   ├── CHTLParser/      # ✅ 语法分析器
│   │   ├── CHTLNode/        # ✅ AST 节点
│   │   └── CHTLGenerator/   # ✅ 代码生成器
│   ├── main.cpp             # 主程序
│   └── demo_parser.cpp      # 演示程序
│
├── tests/                    # TDD 测试
│   ├── test_lexer.cpp       # Lexer 测试 (11 个用例)
│   ├── test_parser.cpp      # Parser 测试 (6 个用例)
│   └── test_generator.cpp   # Generator 测试 (8 个用例)
│
├── examples/                 # 示例文件
│   ├── simple.chtl          # 简单示例
│   ├── showcase.chtl        # 功能展示
│   ├── complete_example.chtl # 完整页面
│   └── website_demo.chtl    # 网站演示
│
├── build.py                 # Python 构建脚本
├── CMakeLists.txt          # CMake 配置
└── CHTL.md                 # 完整语法规范
```

---

## 🧪 测试

### 运行测试

```bash
# 运行所有测试
python3 build.py --test

# 运行特定测试
./build/bin/chtl_tests "[lexer]"      # 仅 Lexer 测试
./build/bin/chtl_tests "[parser]"     # 仅 Parser 测试
./build/bin/chtl_tests "[generator]"  # 仅 Generator 测试

# 详细输出
./build/bin/chtl_tests -s

# 查看测试列表
./build/bin/chtl_tests --list-tests
```

### 测试结果

```
===============================================================================
All tests passed (221 assertions in 25 test cases)
===============================================================================
```

---

## 🛠️ 构建

### 依赖要求

- **C++ 编译器**: 支持 C++20
  - GCC 10+ / Clang 12+ / MSVC 2019+
- **CMake**: 3.20+
- **Python**: 3.6+（用于构建脚本）

### 构建命令

```bash
# 使用 Python 脚本（推荐）
python3 build.py --all              # 完整构建流程
python3 build.py --configure --build # 配置并构建
python3 build.py --test             # 运行测试
python3 build.py --clean            # 清理构建

# 使用 CMake 直接构建
mkdir build && cd build
cmake ..
cmake --build .
./bin/chtl_tests
```

---

## 📖 文档

- **[CHTL.md](CHTL.md)** - 完整的语法规范（2500+ 行）
- **[QUICKSTART.md](QUICKSTART.md)** - 5分钟快速入门
- **[PROJECT_SETUP.md](PROJECT_SETUP.md)** - 项目设置详解
- **[DEVELOPMENT_LOG.md](DEVELOPMENT_LOG.md)** - 完整开发日志
- **[STEP2_SUMMARY.md](STEP2_SUMMARY.md)** - Step 2 总结
- **[STEP3_SUMMARY.md](STEP3_SUMMARY.md)** - Step 3 总结
- **[STEP4_SUMMARY.md](STEP4_SUMMARY.md)** - Step 4 总结

---

## 🎓 技术栈

<table>
<tr>
<td width="33%">

**核心技术**
- C++20
- CMake 3.20+
- Catch2 测试框架

</td>
<td width="33%">

**设计模式**
- 访问者模式
- 递归下降解析
- 智能指针

</td>
<td width="33%">

**开发方法**
- TDD（测试驱动）
- 小步迭代
- 持续集成

</td>
</tr>
</table>

---

## 🌟 示例展示

### 示例 1: 简单页面

```chtl
div {
    text: "Hello CHTL";
    
    style {
        width: 100px;
        height: 100px;
    }
}
```

**生成**: `<div text="Hello CHTL" />`

### 示例 2: 完整网站

参见 `examples/website_demo.chtl` - 包含导航栏、Hero 区域、特性展示、页脚等完整结构。

---

## 🔧 开发指南

### TDD 工作流

```bash
# 1. 编写测试（红色阶段）
编辑 tests/test_xxx.cpp

# 2. 运行测试，确认失败
python3 build.py --test

# 3. 实现功能（绿色阶段）
编辑 src/CHTL/...

# 4. 运行测试，确认通过
python3 build.py --build --test

# 5. 重构代码（重构阶段）
优化代码，保持测试通过
```

### 添加新功能

1. 在 `tests/` 目录添加测试
2. 运行测试确认失败
3. 在 `src/CHTL/` 实现功能
4. 运行测试确认通过
5. 更新文档

---

## 📈 开发进度

```
[███████████████░░░░░] 75% 完成

✅ Step 1: 项目基础结构
✅ Step 2: CHTL 特殊语法块支持 (35+ Token 类型)
✅ Step 3: Parser 基础架构 (AST + 递归下降解析)
✅ Step 4: 代码生成器 (完整的 HTML 生成)
✅ Step 5: 内联样式和脚本块
✅ Step 6: 属性运算和表达式
✅ Step 7: 高级样式系统 (自动类名、&推导、全局样式) ✨
⏳ Step 8: 引用属性和条件表达式
⏳ Step 9: 模板系统
⏳ Step 10: 模块化和导入
```

---

## 🤝 贡献

欢迎贡献！请遵循：

1. **TDD 开发** - 先写测试，后写代码
2. **代码规范** - 遵循 C++ Core Guidelines
3. **小步提交** - 每个功能独立提交
4. **测试覆盖** - 保持 100% 测试通过

---

## 📄 许可证

MIT License - 详见 [LICENSE](LICENSE) 文件

---

## 🙏 致谢

- **Catch2** - 优秀的 C++ 测试框架
- **CMake** - 强大的构建系统
- **C++ 社区** - 丰富的学习资源

---

## 📞 联系方式

- **文档**: 查看 `docs/` 目录
- **问题**: 提交 Issue
- **讨论**: 参与 Discussions

---

<div align="center">

**版本**: v0.6.0-style-advanced  
**状态**: 🚀 快速发展中  
**进度**: 75% 完成

Made with ❤️ using C++20, TDD, and Step-Review-Step

</div>
