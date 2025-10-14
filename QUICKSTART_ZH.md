# CHTL项目快速开始指南

## 一分钟快速体验

```bash
# 1. 构建项目
python3 build.py all

# 2. 运行测试
python3 build.py test

# 3. 运行编译器（演示模式）
python3 build.py run -- examples/simple.chtl
```

## 详细步骤

### 步骤1: 验证环境

确保您的系统满足以下要求：

```bash
# 检查C++编译器（需要支持C++20）
g++ --version    # GCC 10+ 或
clang++ --version # Clang 10+

# 检查CMake版本
cmake --version  # 需要 3.20+

# 检查Python版本
python3 --version # 需要 3.6+
```

### 步骤2: 配置项目

```bash
# 配置Debug版本（默认）
python3 build.py configure

# 或配置Release版本
python3 build.py --build-type Release configure
```

### 步骤3: 编译项目

```bash
# 编译所有目标
python3 build.py build

# 编译完成后会显示：
# CHTL编译器已构建: /workspace/build/bin/chtl
```

### 步骤4: 运行测试

```bash
# 运行所有单元测试
python3 build.py test

# 预期输出：
# ===============================================================================
# All tests passed (41 assertions in 2 test cases)
# 所有测试通过！
```

### 步骤5: 使用编译器

```bash
# 运行编译器（当前版本为演示模式）
./build/bin/chtl examples/simple.chtl

# 或使用构建脚本
python3 build.py run -- examples/simple.chtl
```

## 常用命令

### 开发工作流

```bash
# 完整的开发周期
python3 build.py clean configure build test

# 或简化为
python3 build.py clean all
```

### 仅重新编译

```bash
# 如果已经配置过，只需要
python3 build.py build
```

### 调试模式

```bash
# 使用Debug模式构建（默认）
python3 build.py --build-type Debug build

# 使用Release模式构建
python3 build.py --build-type Release build
```

### 自定义构建目录

```bash
# 使用自定义构建目录
python3 build.py --build-dir my_build configure build
```

## 目录说明

```
构建后的目录结构：
build/
├── bin/
│   ├── chtl          # CHTL编译器可执行文件
│   └── chtl_tests    # 测试可执行文件
├── lib/
│   └── libchtl_lib.a # CHTL静态库
└── CMakeFiles/       # CMake生成的文件
```

## 示例文件

查看 `examples/simple.chtl` 了解CHTL语法：

```chtl
html
{
    body
    {
        div
        {
            style
            {
                width: 100px;
                height: 100px;
            }
            
            text
            {
                "Hello, CHTL!"
            }
        }
    }
}
```

## 测试开发

### 添加新测试

1. 在 `tests/` 目录创建测试文件：

```cpp
#include "catch.hpp"
#include "CHTL/YourModule/YourClass.h"

TEST_CASE("测试描述", "[tag]") {
    // 测试代码
    REQUIRE(condition == true);
}
```

2. 重新构建和测试：

```bash
python3 build.py build test
```

## 故障排除

### 问题1: CMake版本过低

```bash
# 错误信息: CMake 3.20 or higher is required
# 解决方法: 升级CMake
sudo apt-get update
sudo apt-get install cmake
```

### 问题2: C++编译器不支持C++20

```bash
# 错误信息: unsupported option '-std=c++20'
# 解决方法: 升级编译器
sudo apt-get install g++-10  # 或更高版本
```

### 问题3: 找不到Python3

```bash
# 错误信息: python3: command not found
# 解决方法: 
# Ubuntu/Debian
sudo apt-get install python3

# macOS
brew install python3
```

### 问题4: 构建失败

```bash
# 清理并重新构建
python3 build.py clean all
```

## 进阶使用

### 查看详细构建日志

```bash
# CMake详细输出
cmake --build build --verbose
```

### 运行特定测试

```bash
# 运行测试可执行文件，使用Catch2的过滤器
./build/bin/chtl_tests "[token]"  # 只运行带有[token]标签的测试
```

### 安装编译器

```bash
# 将编译器安装到系统
cmake --install build --prefix /usr/local
```

## 性能建议

### Release构建

对于生产环境或性能测试，使用Release模式：

```bash
python3 build.py --build-type Release clean all
```

### 并行编译

构建脚本会自动使用多核编译，您也可以手动指定：

```bash
cmake --build build -j 8  # 使用8个线程
```

## 下一步

1. 阅读 [README_ZH.md](README_ZH.md) 了解项目详情
2. 查看 [CHTL.md](CHTL.md) 学习CHTL语言规范
3. 查看 [PROJECT_STATUS.md](PROJECT_STATUS.md) 了解开发进度
4. 开始贡献代码！

## 获取帮助

- 查看构建脚本帮助：`python3 build.py --help`
- 提交Issue报告问题
- 查阅语言规范文档

---

祝您使用愉快！🎉
