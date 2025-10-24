# CHTL 示例集合

这个目录包含各种CHTL示例，帮助您学习和使用CHTL。

## 示例列表

### 1. basic_formatting.chtl
展示CHTL代码格式化的示例。

**特性**:
- 自动缩进
- 属性对齐
- 嵌套结构美化
- 样式块格式化

**使用格式化器**:
```bash
# 查看格式化后的代码
./chtl format examples/basic_formatting.chtl

# 保存到文件
./chtl format examples/basic_formatting.chtl --output formatted.chtl

# 检查文件是否格式正确
./chtl format examples/basic_formatting.chtl --check

# 使用2空格缩进
./chtl format examples/basic_formatting.chtl --indent 2

# 使用Tab缩进
./chtl format examples/basic_formatting.chtl --tabs
```

### 2. basic.chtl
基础的CHTL语法示例。

### 3. 更多示例
查看 `/workspace/CHTL/tests/resources/` 目录获取更多测试示例。

## 快速开始

### 编译CHTL文件
```bash
./chtl compile examples/basic_formatting.chtl > output.html
```

### 格式化CHTL文件
```bash
./chtl format examples/basic_formatting.chtl
```

### 序列化CHTL文件
```bash
./chtl serialize examples/basic_formatting.chtl > output.bin
```

## 格式化器选项

| 选项 | 说明 | 示例 |
|------|------|------|
| `--output <file>` | 输出到文件 | `--output formatted.chtl` |
| `--check` | 检查格式 | `--check` |
| `--indent <n>` | 缩进大小 | `--indent 2` |
| `--tabs` | 使用Tab | `--tabs` |

## 编程API

您也可以在C++代码中使用格式化器：

```cpp
#include "formatter/Formatter.h"

// 使用默认选项
CHTL::Formatter formatter;
std::string formatted = formatter.Format(input_code);

// 自定义选项
CHTL::FormatterOptions options;
options.indentSize = 2;
options.useTabs = false;
CHTL::Formatter formatter(options);
std::string formatted = formatter.Format(input_code);
```

## 了解更多

- [CHTL 语言规范](../CHTL.md)
- [贡献指南](../CONTRIBUTING.md)
- [项目路线图](../ROADMAP.md)
