# CHTL 快速入门

## 5 分钟开始使用

### 1. 克隆并构建项目

```bash
# 构建项目（清理 + 配置 + 编译 + 测试）
python3 build.py --all

# 如果一切顺利，你应该看到：
# ✅ CMake 配置完成
# ✅ 构建完成
# ✅ 所有测试通过
```

### 2. 运行你的第一个 CHTL 程序

```bash
# 运行示例文件
python3 build.py --run examples/simple.chtl
```

### 3. 查看 Token 输出

```bash
# 查看词法分析结果
./build/bin/chtl examples/simple.chtl --tokens
```

## 构建脚本命令速查

```bash
# 完整构建流程
python3 build.py --all

# 仅构建
python3 build.py --configure --build

# 仅测试
python3 build.py --test

# 清理重建
python3 build.py --clean --configure --build

# Release 构建
python3 build.py --all --build-type Release

# 列出生成的二进制文件
python3 build.py --list

# 运行编译器
python3 build.py --run <文件.chtl>
```

## 项目结构一览

```
CHTL/
├── build.py              # Python 构建脚本 ⭐
├── CMakeLists.txt        # CMake 配置
├── src/                  # 源代码
│   ├── main.cpp          # 主程序
│   └── CHTL/             # 核心模块
│       └── CHTLLexer/    # 词法分析器
├── tests/                # 测试
│   ├── test_main.cpp
│   └── test_lexer.cpp
├── examples/             # 示例
│   └── simple.chtl
└── third-party/          # 第三方库
    └── catch.hpp         # 测试框架
```

## 创建自己的 CHTL 文件

创建 `test.chtl`:

```chtl
// 我的第一个 CHTL 程序

div {
    text: "Hello, World!";
    
    style {
        color: red;
        font-size: 20px;
    }
}
```

运行它：

```bash
./build/bin/chtl test.chtl --tokens
```

## TDD 开发示例

### 1. 编写测试

在 `tests/test_lexer.cpp` 中添加：

```cpp
TEST_CASE("Lexer - 识别新的关键字", "[lexer]") {
    Lexer lexer("import");
    auto tokens = lexer.tokenize();
    
    REQUIRE(tokens[0].type == TokenType::KEYWORD_IMPORT);
}
```

### 2. 运行测试（失败）

```bash
python3 build.py --test
# ❌ 测试失败（预期）
```

### 3. 实现功能

在 `src/CHTL/CHTLLexer/Token.h` 中添加：

```cpp
enum class TokenType {
    // ...
    KEYWORD_IMPORT,  // 新增
};
```

在 `src/CHTL/CHTLLexer/Lexer.cpp` 中添加：

```cpp
TokenType Lexer::identifierType(const std::string& text) const {
    // ...
    if (text == "import") return TokenType::KEYWORD_IMPORT;
    // ...
}
```

### 4. 运行测试（通过）

```bash
python3 build.py --build --test
# ✅ 所有测试通过
```

## 常见问题

### Q: 如何重新编译？

```bash
python3 build.py --build
```

### Q: 如何清理所有构建产物？

```bash
python3 build.py --clean
```

### Q: 如何只运行特定的测试？

```bash
./build/bin/chtl_tests "[lexer]"  # 只运行带 [lexer] 标签的测试
```

### Q: 如何开启详细的测试输出？

```bash
./build/bin/chtl_tests -s  # -s 表示 success，显示所有通过的测试
```

## 下一步

1. 阅读 `CHTL.md` 了解完整的语法规范
2. 查看 `PROJECT_SETUP.md` 了解详细的项目设置
3. 开始实现你感兴趣的功能！

## 开发提示

- **TDD 优先**: 先写测试，再实现功能
- **小步迭代**: 每次只做一件事，确保测试通过
- **频繁提交**: 功能完成后立即提交
- **代码审查**: 提交前检查代码质量

## 需要帮助？

- 查看 `tests/` 目录中的测试用例作为参考
- 阅读 `src/CHTL/CHTLLexer/` 中的现有实现
- 参考 Catch2 文档: https://github.com/catchorg/Catch2

祝开发愉快！ 🚀
