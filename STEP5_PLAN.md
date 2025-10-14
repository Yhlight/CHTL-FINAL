# Step 5: 局部样式块实现计划

## 📋 基于 CHTL.md 规范的实现计划

根据 CHTL.md 第 103-335 行的详细规范，局部样式块包含以下功能：

---

## 🎯 功能清单（按优先级）

### P0 - 核心功能（立即实现）

#### 1. 内联样式 ✅ 目标
**规范位置**: CHTL.md 109-125 行

**功能描述**:
```chtl
div {
    style {
        width: 100px;
        height: 200px;
    }
}
```

**应生成**:
```html
<div style="width: 100px; height: 200px;"></div>
```

**实现要点**:
- ✅ 解析 style {} 块中的属性
- ✅ 生成 style 属性
- ✅ 正确的 CSS 属性格式
- ✅ 支持所有 CSS 属性

---

### P1 - 重要功能

#### 2. 属性运算 
**规范位置**: CHTL.md 175-202 行

**功能描述**:
```chtl
div {
    style {
        width: 100px + 50px;  // 150px
        height: 200px * 2;    // 400px
    }
}
```

**运算规则**:
- ✅ 算术运算符：+ - * / % **
- ✅ 单位合并（同单位运算）
- ✅ 不同单位报错
- ✅ 左结合/右结合
- ✅ 字符串解耦（如 `linear 0.5s all`）
- ✅ 百分号处理（`1px + 5%`）

---

#### 3. 引用属性
**规范位置**: CHTL.md 203-236 行

**功能描述**:
```chtl
div {
    id: box;
    style {
        width: 100px;
    }
}

div {
    style {
        width: 100px + box.width;  // 200px
    }
}
```

**支持的选择器**:
- ✅ `box` - 自动推断（tag -> id -> class）
- ✅ `.box` - 类选择器
- ✅ `#box` - id 选择器
- ✅ `button` - tag 选择器
- ✅ `.box button` - 后代选择器
- ✅ `button[0]` - 精确访问

**限制**:
- ❌ 仅限局部 style 块
- ❌ 全局 style 不支持

---

#### 4. 属性条件表达式
**规范位置**: CHTL.md 238-334 行

**功能描述**:
```chtl
div {
    style {
        width: 100px;
        height: 100px;
        background-color: width > 50px ? "red" : "blue";
    }
}
```

**特性**:
- ✅ 三元运算符
- ✅ 链式调用（逗号分隔）
- ✅ 可选选项（省略 false 分支）
- ✅ 算术运算符（+ - * / % **）
- ✅ 逻辑运算符（&& ||）
- ✅ 引用属性条件表达式

**示例 - 链式调用**:
```chtl
background-color: 
    width > 50px ? "red" : "blue",
    width > 100px ? "green" : "yellow", 
    height < 100px ? "purple" : "pink";
```

**示例 - 引用属性**:
```chtl
width: .box.width > 50px ? .box.width / 2 : .box.height / 2;
```

---

### P2 - 高级功能（后续）

#### 5. 自动化类名/id
**规范位置**: CHTL.md 127-143 行

```chtl
div {
    style {
        .box {  // 自动添加 class="box"
            width: 300px;
        }
    }
}
```

**特性**:
- 自动添加 class 或 id 属性
- 类/id 选择器代码提升到全局样式块

---

#### 6. 上下文推导（&）
**规范位置**: CHTL.md 145-173 行

```chtl
div {
    style {
        .box { }
        
        &:hover {  // & 推导为 .box
        }
        
        &::before {
        }
    }
}
```

---

## 🔧 实现步骤

### 阶段 1: 内联样式（当前）

#### 1.1 创建 StyleNode
```cpp
class StyleNode : public BaseNode {
private:
    std::map<std::string, std::string> properties_;
    
public:
    void addProperty(const std::string& name, const std::string& value);
    const std::map<std::string, std::string>& getProperties() const;
};
```

#### 1.2 增强 Parser
```cpp
NodePtr Parser::parseStyleBlock() {
    // 解析 style { property: value; }
    auto styleNode = std::make_unique<StyleNode>();
    
    while (/* 属性 */) {
        std::string propName = parseCSSPropertyName();
        std::string propValue = parseCSSPropertyValue();
        styleNode->addProperty(propName, propValue);
    }
    
    return styleNode;
}
```

#### 1.3 增强 Generator
```cpp
void Generator::visit(ElementNode& node) {
    // 检查是否有 StyleNode 子节点
    // 如果有，收集所有 CSS 属性
    // 生成 style="..." 属性
}
```

#### 1.4 TDD 测试
```cpp
TEST_CASE("Style - 内联样式生成") {
    std::string source = R"(
        div {
            style {
                width: 100px;
                height: 200px;
            }
        }
    )";
    
    // 应生成: <div style="width: 100px; height: 200px;"></div>
}
```

---

### 阶段 2: 属性运算

#### 2.1 表达式解析器
```cpp
class CSSExpressionParser {
public:
    std::string evaluate(const std::string& expr);
    
private:
    double parseNumber(const std::string& str, std::string& unit);
    std::string formatValue(double value, const std::string& unit);
};
```

#### 2.2 支持的运算
- 加法：`100px + 50px` → `150px`
- 减法：`200px - 50px` → `150px`
- 乘法：`100px * 2` → `200px`
- 除法：`200px / 2` → `100px`
- 取模：`100px % 30px` → `10px`
- 幂运算：`2px ** 3` → `8px`

---

### 阶段 3: 引用属性

#### 3.1 选择器解析
```cpp
class PropertyReference {
    std::string selector;  // ".box", "#id", "tag"
    std::string property;  // "width", "height"
};
```

#### 3.2 属性查找
```cpp
std::string findPropertyValue(
    const AST& ast,
    const PropertyReference& ref
);
```

---

### 阶段 4: 条件表达式

#### 4.1 三元运算符
```cpp
class ConditionalExpression {
    std::string condition;   // "width > 50px"
    std::string trueValue;   // "red"
    std::string falseValue;  // "blue"
};
```

#### 4.2 链式调用
```cpp
std::vector<ConditionalExpression> expressions;
// 从第一个开始求值，第一个为 true 就返回
```

---

## 📊 测试计划

### 基础测试
- ✅ 单个 CSS 属性
- ✅ 多个 CSS 属性
- ✅ 各种 CSS 单位（px, em, rem, %, vh, vw）
- ✅ 特殊 CSS 属性（color, background, border）

### 运算测试
- ✅ 基本运算符
- ✅ 单位合并
- ✅ 运算优先级
- ✅ 括号表达式

### 引用测试
- ✅ id 选择器引用
- ✅ class 选择器引用
- ✅ tag 选择器引用
- ✅ 引用不存在的属性（错误处理）

### 条件测试
- ✅ 简单三元运算
- ✅ 链式条件
- ✅ 逻辑运算符
- ✅ 引用属性条件

---

## 🎯 成功标准

### 代码质量
- ✅ 零警告编译
- ✅ 100% 测试通过
- ✅ 完整的错误处理
- ✅ 清晰的代码注释

### 功能完整性
- ✅ 严格遵循 CHTL.md 规范
- ✅ 所有示例代码可运行
- ✅ 边界情况处理
- ✅ 错误消息友好

### 性能
- ✅ 表达式求值高效
- ✅ 选择器查找优化
- ✅ 内存使用合理

---

## 📝 当前任务

**立即开始**: 阶段 1 - 内联样式实现

**步骤**:
1. ✅ 创建 StyleNode
2. ✅ 增强 Parser 解析 style 块
3. ✅ 增强 Generator 生成 style 属性
4. ✅ 编写 TDD 测试
5. ✅ 验证所有测试通过

**预期结果**:
```chtl
div {
    style {
        width: 100px;
        height: 200px;
        color: red;
    }
}
```
→
```html
<div style="width: 100px; height: 200px; color: red;"></div>
```

---

**开始日期**: 2025-10-14  
**预计完成**: 阶段 1（1-2小时），阶段 2-4（分阶段进行）  
**遵循规范**: CHTL.md 严格遵守
