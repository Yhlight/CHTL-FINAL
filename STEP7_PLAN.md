# Step 7 计划 - 样式系统增强

**版本**: v0.6.0-style-advanced  
**开始日期**: 2025-10-15  
**预计完成**: 2025-11-05  
**优先级**: ⭐⭐⭐⭐⭐ 最高

---

## 🎯 目标

实现 CHTL 样式系统的核心高级功能：
1. **自动化类名/id**：在 style 块中使用 `.box` 自动添加类名
2. **上下文推导&**：使用 `&` 引用当前元素并支持伪类/伪元素
3. **全局样式块**：自动生成 `<style>` 标签并注入到 `<head>`

这些功能是 CHTL 区别于普通 HTML 的核心特性，也是 CHTL.md 规范中的必需功能。

---

## 📋 Step 7.1: 自动化类名/id

### 功能描述

**输入 (CHTL)**:
```chtl
div {
    style {
        .box {
            width: 300px;
            height: 200px;
        }
    }
}
```

**输出 (HTML)**:
```html
<div class="box" />
<style>
  .box {
    width: 300px;
    height: 200px;
  }
</style>
```

### 技术设计

#### 1. Lexer 扩展

**新增 Token 类型**:
```cpp
// TokenType 枚举中添加
DOT,              // .
HASH,             // #（已有，用于注释，需要区分上下文）
```

**Lexer 修改**:
- 在 `nextToken()` 中识别 `.` 作为独立 token
- 区分 `#` 作为注释 vs 选择器（基于上下文）

#### 2. Parser 扩展

**新增数据结构**:
```cpp
// 在 Parser.h 中
struct CSSSelector {
    enum Type {
        CLASS,      // .box
        ID,         // #box
        PSEUDO,     // :hover
        PSEUDO_ELEM // ::before
    };
    
    Type type;
    std::string name;
};

struct CSSRule {
    std::vector<CSSSelector> selectors;
    std::map<std::string, std::string> properties;
};
```

**新增方法**:
```cpp
// 解析 CSS 选择器
CSSSelector parseSelector();

// 解析 CSS 规则块
CSSRule parseStyleRule();

// 修改现有的 parseStyleBlock
NodePtr parseStyleBlock();  // 需要处理两种情况：内联样式和规则块
```

**解析逻辑**:
1. 在 `parseStyleBlock()` 中检测下一个 token
2. 如果是 `.` 或 `#`，则进入 CSS 规则模式
3. 解析选择器和规则块
4. 提取选择器名称，自动添加到元素的 `class` 或 `id` 属性
5. 将 CSS 规则添加到全局样式收集器

#### 3. GlobalStyleCollector

**新增文件**: `src/CHTL/CHTLCommon/GlobalStyleCollector.h`

```cpp
class GlobalStyleCollector {
public:
    static GlobalStyleCollector& getInstance();
    
    // 添加 CSS 规则
    void addRule(const std::string& selector, 
                 const std::map<std::string, std::string>& properties);
    
    // 获取所有规则（用于生成 <style> 标签）
    std::string generateCSS(bool prettyPrint = true) const;
    
    // 清空（每次编译前）
    void clear();
    
    // 合并重复选择器
    void merge();
    
private:
    GlobalStyleCollector() = default;
    std::map<std::string, std::map<std::string, std::string>> rules_;
};
```

#### 4. ElementNode 扩展

**修改**: `src/CHTL/CHTLNode/ElementNode.h`

```cpp
// 添加方法
void addAutoClass(const std::string& className);
void addAutoId(const std::string& idName);
bool hasClass() const;
bool hasId() const;
```

#### 5. Generator 扩展

**修改**: `src/CHTL/CHTLGenerator/Generator.cpp`

在 `generateDocument()` 中：
1. 检查是否有 `<html>` 节点
2. 查找或创建 `<head>` 节点
3. 在 `<head>` 中注入全局样式

```cpp
void Generator::generateDocument(BaseNode& root, const std::string& doctype) {
    // 1. 生成 DOCTYPE
    if (!doctype.empty()) {
        write(doctype);
        write("\n");
    }
    
    // 2. 遍历根节点
    root.accept(*this);
    
    // 3. 如果有全局样式，注入到 <head>
    if (GlobalStyleCollector::getInstance().hasRules()) {
        injectGlobalStyles(root);
    }
}

void Generator::injectGlobalStyles(BaseNode& root) {
    // 查找 <html> -> <head>
    // 如果不存在则创建
    // 注入 <style> 标签
}
```

### 测试用例

**test_auto_class.cpp**: 新增测试文件

```cpp
TEST_CASE("Auto Class - Basic", "[style][auto-class]") {
    // 测试基础的 .box 自动添加类名
}

TEST_CASE("Auto Class - Multiple Classes", "[style][auto-class]") {
    // 测试多个类选择器
}

TEST_CASE("Auto ID - Basic", "[style][auto-class]") {
    // 测试 #box 自动添加 id
}

TEST_CASE("Auto Class - With Inline Style", "[style][auto-class]") {
    // 测试同时有内联样式和类选择器
}

TEST_CASE("Auto Class - Nested Elements", "[style][auto-class]") {
    // 测试嵌套元素的类自动添加
}

TEST_CASE("Auto Class - CSS Generation", "[style][auto-class]") {
    // 测试生成的 CSS 格式正确
}
```

**预计**: 6-8 个测试用例，40-50 个断言

### 实现步骤

1. **阶段 1**: Lexer 和 Token（30分钟）
   - 添加 DOT token 类型
   - 修改 Lexer::nextToken()
   
2. **阶段 2**: 数据结构（30分钟）
   - 创建 CSSSelector 和 CSSRule 结构
   - 创建 GlobalStyleCollector 类
   
3. **阶段 3**: Parser（2小时）
   - 实现 parseSelector()
   - 实现 parseStyleRule()
   - 修改 parseStyleBlock()
   
4. **阶段 4**: ElementNode 修改（30分钟）
   - 添加 addAutoClass/addAutoId 方法
   
5. **阶段 5**: Generator（1.5小时）
   - 实现样式注入逻辑
   - 实现 CSS 生成
   
6. **阶段 6**: 测试（1.5小时）
   - 编写所有测试用例
   - 验证功能正确性

**总计**: 约 6.5 小时

---

## 📋 Step 7.2: 上下文推导&

### 功能描述

**输入 (CHTL)**:
```chtl
div {
    style {
        .box {
            width: 300px;
        }
        
        &:hover {
            width: 350px;
        }
        
        &::before {
            content: ">";
        }
    }
}
```

**输出 (HTML)**:
```html
<div class="box" />
<style>
  .box {
    width: 300px;
  }
  .box:hover {
    width: 350px;
  }
  .box::before {
    content: ">";
  }
</style>
```

### 技术设计

#### 1. Lexer 扩展

**新增 Token 类型**:
```cpp
AMPERSAND,        // &
DOUBLE_COLON,     // ::
```

#### 2. Parser 扩展

**新增方法**:
```cpp
// 解析 & 选择器
CSSSelector parseAmpersandSelector();

// 上下文跟踪
struct StyleContext {
    std::string currentClass;  // 当前元素的类名
    std::string currentId;     // 当前元素的 id
};
StyleContext styleContext_;
```

**解析逻辑**:
1. 在解析 `.box {}` 时，记录当前类名到 `styleContext_`
2. 在解析 `&` 时，查找 `styleContext_` 获取当前类名/id
3. 支持伪类（`:hover`, `:active`, `:focus` 等）
4. 支持伪元素（`::before`, `::after`, `::first-line` 等）
5. 替换 `&` 为实际的类名/id

#### 3. 伪类/伪元素支持

**支持的伪类**:
- `:hover`, `:active`, `:focus`, `:visited`
- `:first-child`, `:last-child`, `:nth-child()`
- `:checked`, `:disabled`, `:enabled`

**支持的伪元素**:
- `::before`, `::after`
- `::first-line`, `::first-letter`
- `::selection`

### 测试用例

```cpp
TEST_CASE("Ampersand - With Class", "[style][ampersand]") {
    // 测试 & 与类选择器结合
}

TEST_CASE("Ampersand - Pseudo Class", "[style][ampersand]") {
    // 测试 &:hover 等伪类
}

TEST_CASE("Ampersand - Pseudo Element", "[style][ampersand]") {
    // 测试 &::before 等伪元素
}

TEST_CASE("Ampersand - Multiple Rules", "[style][ampersand]") {
    // 测试多个 & 规则
}

TEST_CASE("Ampersand - With ID", "[style][ampersand]") {
    // 测试 & 与 id 选择器结合
}

TEST_CASE("Ampersand - Nested", "[style][ampersand]") {
    // 测试嵌套情况下的 &
}
```

**预计**: 6-8 个测试用例，40-50 个断言

### 实现步骤

1. **阶段 1**: Lexer（30分钟）
   - 添加 AMPERSAND 和 DOUBLE_COLON token
   
2. **阶段 2**: Parser（2小时）
   - 实现上下文跟踪
   - 实现 & 解析和替换
   - 支持伪类和伪元素
   
3. **阶段 3**: 测试（1.5小时）
   - 编写测试用例
   - 验证功能

**总计**: 约 4 小时

---

## 📋 Step 7.3: 全局样式块生成

### 功能描述

**输入 (CHTL)**:
```chtl
html {
    body {
        div {
            style {
                .box {
                    width: 100px;
                }
            }
        }
        div {
            style {
                .container {
                    width: 200px;
                }
            }
        }
    }
}
```

**输出 (HTML)**:
```html
<html>
  <head>
    <style>
      .box {
        width: 100px;
      }
      .container {
        width: 200px;
      }
    </style>
  </head>
  <body>
    <div class="box" />
    <div class="container" />
  </body>
</html>
```

### 技术设计

#### 1. GlobalStyleCollector 增强

```cpp
class GlobalStyleCollector {
public:
    // 生成格式化的 CSS
    std::string generateCSS(bool prettyPrint = true, int indent = 0) const;
    
    // 合并重复选择器的属性
    void merge();
    
    // 按字母顺序排序选择器
    void sort();
    
    // 检查是否有规则
    bool hasRules() const;
};
```

#### 2. Generator 增强

**新增方法**:
```cpp
// 查找 <head> 元素
ElementNode* findHeadElement(BaseNode& root);

// 创建 <head> 元素
ElementNode* createHeadElement();

// 注入全局样式到 <head>
void injectGlobalStyles(BaseNode& root);

// 生成 <style> 标签
std::string generateStyleTag();
```

**注入策略**:
1. 遍历 AST 查找 `<html>` 元素
2. 在 `<html>` 中查找 `<head>` 元素
3. 如果不存在 `<head>`，在 `<body>` 之前创建
4. 在 `<head>` 中插入 `<style>` 节点
5. 生成完整的 CSS 内容

#### 3. 样式合并

**合并规则**:
```cpp
// 示例：
// .box { width: 100px; }
// .box { height: 200px; }
// 合并为：
// .box { width: 100px; height: 200px; }
```

**冲突处理**:
- 相同属性，后者覆盖前者
- 保持声明顺序

### 测试用例

```cpp
TEST_CASE("Global Style - Basic Injection", "[style][global]") {
    // 测试基础的样式注入
}

TEST_CASE("Global Style - Multiple Rules", "[style][global]") {
    // 测试多个样式规则
}

TEST_CASE("Global Style - Auto Create Head", "[style][global]") {
    // 测试自动创建 <head>
}

TEST_CASE("Global Style - Merge Duplicate Selectors", "[style][global]") {
    // 测试合并重复选择器
}

TEST_CASE("Global Style - CSS Formatting", "[style][global]") {
    // 测试 CSS 格式化
}

TEST_CASE("Global Style - Compact Mode", "[style][global]") {
    // 测试紧凑模式
}
```

**预计**: 6-8 个测试用例，40-50 个断言

### 实现步骤

1. **阶段 1**: GlobalStyleCollector 增强（1小时）
   - 实现 generateCSS()
   - 实现 merge() 和 sort()
   
2. **阶段 2**: Generator 注入逻辑（2小时）
   - 实现 findHeadElement()
   - 实现 injectGlobalStyles()
   - 处理各种边界情况
   
3. **阶段 3**: 测试（1.5小时）
   - 编写测试用例
   - 验证功能

**总计**: 约 4.5 小时

---

## 📊 总体统计

### 时间预估

| 任务 | 预计时间 |
|------|---------|
| Step 7.1: 自动化类名/id | 6.5 小时 |
| Step 7.2: 上下文推导& | 4 小时 |
| Step 7.3: 全局样式块 | 4.5 小时 |
| 集成测试 | 2 小时 |
| 文档更新 | 2 小时 |
| **总计** | **19 小时** |

### 代码规模预估

| 模块 | 新增代码 | 修改代码 |
|------|---------|---------|
| Lexer | 50 行 | 20 行 |
| Parser | 400 行 | 100 行 |
| Generator | 200 行 | 50 行 |
| GlobalStyleCollector | 150 行 | - |
| 测试 | 600 行 | - |
| **总计** | **1400 行** | **170 行** |

### 测试覆盖预估

| 模块 | 测试用例 | 断言数 |
|------|---------|--------|
| 自动化类名/id | 6-8 | 40-50 |
| 上下文推导& | 6-8 | 40-50 |
| 全局样式块 | 6-8 | 40-50 |
| **总计** | **18-24** | **120-150** |

### 完成后指标

- **测试用例**: 56 → 74-80
- **断言数量**: 380 → 500-530
- **代码规模**: ~6,500 → ~8,000 行
- **规范符合度**: 60% → 75%

---

## 🎯 成功标准

### 功能标准
- [x] 所有 Step 7.1-7.3 功能完整实现
- [ ] 所有测试用例 100% 通过
- [ ] 零编译警告
- [ ] 符合 CHTL.md 规范

### 质量标准
- [ ] 代码可读性高，注释完整
- [ ] 错误处理完善
- [ ] 性能符合预期（编译速度不下降超过 10%）
- [ ] 内存使用合理（不增加超过 20%）

### 文档标准
- [ ] 更新 README.md
- [ ] 更新 CHANGELOG.md
- [ ] 创建 STEP7_SUMMARY.md
- [ ] 创建示例文件（examples/auto_class_demo.chtl）
- [ ] 更新 TUTORIAL.md

---

## 📝 注意事项

### 兼容性
- 保持向后兼容：现有的 CHTL 文件应该仍然能正常编译
- 新功能是可选的：不使用 `.box` 等语法，行为不变

### 性能
- GlobalStyleCollector 使用单例模式，避免重复创建
- 样式合并在编译后期进行，避免影响解析性能
- CSS 生成使用高效的字符串拼接

### 安全性
- 验证类名和 id 的合法性（避免注入）
- CSS 属性值的安全转义
- 限制 CSS 规则数量（防止 DoS）

---

## 🚀 开始实施

准备就绪，开始实施 Step 7.1！

**当前状态**: ✅ 计划完成  
**下一步**: 实现自动化类名/id 功能

---

**文档版本**: v1.0  
**创建日期**: 2025-10-15  
**最后更新**: 2025-10-15
