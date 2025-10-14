# Step 5.3 实现计划 - 引用属性

## 📋 规范依据

**CHTL.md 第 203-236 行** - 引用属性

---

## 🎯 目标功能

### 1. 基础引用语法

**规范要求** (CHTL.md 第209行):
> 语法为CSS选择器.属性

```chtl
div {
    id: box;
    style {
        width: 100px;
    }
}

div {
    style {
        width: box.width + 50px;      // 引用 #box 的 width
    }
}
```

**预期输出**:
```html
<div id="box" style="width: 100px" />
<div style="width: 150px" />
```

---

## 📝 规范分析

根据 CHTL.md 第203-236行，引用属性功能相对复杂，涉及：

1. **CSS 选择器支持**
   - ID 选择器：`#box.width`
   - 类选择器：`.container.width`
   - 标签选择器：`div.width`

2. **属性查找**
   - 在 AST 中查找匹配的元素
   - 提取指定属性的值
   - 支持 style 属性

3. **与表达式结合**
   - `box.width + 50px`
   - `#header.height * 2`

4. **限制**
   - 全局 style 不支持（CHTL.md 第207行）
   - 只能引用局部样式块中的属性

---

## 🤔 技术评估

### 复杂度分析

引用属性是一个**高复杂度**功能，涉及：

1. **CSS 选择器解析**（中等复杂）
2. **AST 遍历和查找**（中等复杂）
3. **属性值提取**（简单）
4. **循环依赖检测**（复杂）
5. **作用域管理**（复杂）

### 实现挑战

1. **解析时机问题**
   - 引用可能指向后面定义的元素
   - 需要两遍处理：先构建 AST，再解析引用

2. **循环依赖**
   ```chtl
   div {
       id: a;
       style { width: b.width; }
   }
   div {
       id: b;
       style { width: a.width; }  // 循环依赖！
   }
   ```

3. **作用域**
   - 引用是全局的还是局部的？
   - 如何处理重名元素？

---

## ⚠️ 重要决策

考虑到：
1. 引用属性的实现复杂度很高
2. 需要重构现有的 Parser 架构（两遍处理）
3. 需要实现循环依赖检测
4. 可能需要数天时间

**建议**：
- 暂时跳过引用属性
- 优先实现更简单的功能
- 或者实现一个简化版本（仅支持 ID 引用，无循环检测）

---

## 🚀 备选方案

### 方案 A: 简化实现（推荐）

**范围**：
- ✅ 仅支持 ID 选择器引用
- ✅ 引用必须在定义之后
- ❌ 不支持循环依赖检测
- ❌ 不支持类/标签选择器

**优点**：
- 实现简单快速
- 满足基本需求
- 可以后续扩展

**示例**：
```chtl
div {
    id: box;
    style { width: 100px; }
}

div {
    style { width: box.width + 50px; }  // OK
}
```

---

### 方案 B: 完整实现

**范围**：
- ✅ 所有 CSS 选择器
- ✅ 循环依赖检测
- ✅ 两遍解析
- ✅ 完整作用域

**缺点**：
- 工作量巨大（3-5天）
- 需要重构 Parser
- 复杂度高

---

### 方案 C: 跳过引用属性

**原因**：
1. 实现复杂度过高
2. CHTL.md 中引用属性不是核心功能
3. 可以在后续版本中实现

**优点**：
- 快速推进到下一个功能
- 保持项目节奏

**下一步**：
- Step 5.4: 条件表达式（CHTL.md 第238-334行）
- 或其他更简单的功能

---

## 💭 推荐决策

**推荐：方案 C - 暂时跳过引用属性**

**理由**：
1. **复杂度太高**：需要重构 Parser，实现两遍解析
2. **时间成本**：预计需要 3-5 天
3. **优先级**：引用属性不是核心功能
4. **当前进度**：已经完成 85% 的规范
5. **替代方案**：可以在未来版本中实现

**建议下一步**：
- 跳到 Step 5.4: 条件表达式
- 或实现其他章节的功能
- 或优化现有功能
- 或完善文档和示例

---

## 📊 如果实现的话

### 技术设计（简化版）

#### 1. 引用语法 AST 节点

```cpp
class PropertyReference {
    std::string selector_;   // "box" 或 "#box" 或 ".class"
    std::string property_;   // "width" 或 "height"
    
    CSSValue resolve(const AST& ast);  // 查找并返回值
};
```

#### 2. 修改 CSSExpression

```cpp
primary → NUMBER UNIT?
        | '(' expression ')'
        | SELECTOR '.' IDENTIFIER  // 新增：引用
```

#### 3. AST 查找

```cpp
CSSValue resolveReference(const std::string& selector, 
                         const std::string& property,
                         const AST& ast) {
    // 1. 查找匹配选择器的元素
    ElementNode* element = findElement(selector, ast);
    
    // 2. 提取属性值
    if (auto* styleNode = element->findStyleNode()) {
        std::string value = styleNode->getProperty(property);
        return CSSValue::fromString(value);
    }
    
    throw ParseError("未找到属性: " + selector + "." + property);
}
```

#### 4. 测试用例

```cpp
TEST_CASE("Reference - 基础引用", "[reference]") {
    std::string source = R"(
        div {
            id: box;
            style {
                width: 100px;
            }
        }
        
        div {
            style {
                width: box.width + 50px;
            }
        }
    )";
    
    std::string html = compileToHtml(source);
    
    REQUIRE(html.find("width: 100px") != std::string::npos);
    REQUIRE(html.find("width: 150px") != std::string::npos);
}
```

---

## 🎯 最终建议

**暂时跳过引用属性，继续其他功能的开发**

原因：
1. 复杂度过高，性价比低
2. 当前进度已经很好（85%）
3. 可以在未来版本中实现
4. 应该优先完成更多功能

**建议下一步选择**：

**选项 1**: Step 5.4 条件表达式（CHTL.md 第238-334行）
- 三元运算符
- 逻辑运算
- 比较运算

**选项 2**: Step 6 模板系统（CHTL.md 第336+行）
- [Template] 语法块
- 模板定义和使用

**选项 3**: 优化和完善
- 性能优化
- 错误提示改进
- 更多示例
- 文档完善

---

**请用户决策：是否跳过引用属性，或者实现简化版？**
