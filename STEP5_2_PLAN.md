# Step 5.2 实现计划 - 属性运算

## 📋 规范依据

**CHTL.md 第 175-202 行** - 属性运算

---

## 🎯 目标功能

### 1. 基础算术运算

**规范要求**:
```chtl
div {
    style {
        width: 100px + 50px;      // → 150px
        height: 200px * 2;        // → 400px
        margin: 50px - 10px;      // → 40px
        padding: 100px / 4;       // → 25px
    }
}
```

**支持的运算符**:
- `+` 加法
- `-` 减法
- `*` 乘法
- `/` 除法
- `%` 取模
- `**` 幂运算

---

## 🏗️ 技术设计

### 1. CSSValue 类设计

**功能**: 表示带单位的 CSS 值

```cpp
class CSSValue {
private:
    double value_;      // 数值
    std::string unit_;  // 单位（px, em, rem, %等）
    
public:
    CSSValue(double value, std::string unit = "");
    
    // 算术运算（单位合并验证）
    CSSValue operator+(const CSSValue& other) const;
    CSSValue operator-(const CSSValue& other) const;
    CSSValue operator*(const CSSValue& other) const;
    CSSValue operator/(const CSSValue& other) const;
    
    // 转换为字符串
    std::string toString() const;
    
    // 单位验证
    bool canMergeWith(const CSSValue& other) const;
};
```

**单位合并规则**:
- 相同单位可以加减：`100px + 50px = 150px`
- 数字可以乘除任意单位：`100px * 2 = 200px`
- 不同单位不能加减：`100px + 50em` → 错误

---

### 2. 表达式解析器设计

**递归下降解析**:

```
expression  → term (('+' | '-') term)*
term        → factor (('*' | '/' | '%') factor)*
factor      → unary ('**' unary)?
unary       → ('-' | '+')? primary
primary     → NUMBER UNIT?
            | '(' expression ')'
```

**示例解析**:
```
100px + 50px * 2
→ 100px + (50px * 2)
→ 100px + 100px
→ 200px
```

---

### 3. 表达式求值器

**CSSExpression 类**:

```cpp
class CSSExpression {
public:
    static CSSValue evaluate(const std::string& expr, Lexer& lexer);
    
private:
    static CSSValue parseExpression();
    static CSSValue parseTerm();
    static CSSValue parseFactor();
    static CSSValue parseUnary();
    static CSSValue parsePrimary();
};
```

---

## 📝 TDD 测试用例

### 测试 1: 基础算术运算

```cpp
TEST_CASE("Expression - 基础算术", "[expression]") {
    SECTION("加法") {
        auto value = evaluate("100px + 50px");
        REQUIRE(value.toString() == "150px");
    }
    
    SECTION("减法") {
        auto value = evaluate("100px - 30px");
        REQUIRE(value.toString() == "70px");
    }
    
    SECTION("乘法") {
        auto value = evaluate("50px * 2");
        REQUIRE(value.toString() == "100px");
    }
    
    SECTION("除法") {
        auto value = evaluate("100px / 4");
        REQUIRE(value.toString() == "25px");
    }
}
```

---

### 测试 2: 运算符优先级

```cpp
TEST_CASE("Expression - 优先级", "[expression]") {
    SECTION("乘法优先于加法") {
        auto value = evaluate("100px + 50px * 2");
        REQUIRE(value.toString() == "200px"); // 100 + (50*2)
    }
    
    SECTION("除法优先于减法") {
        auto value = evaluate("100px - 50px / 2");
        REQUIRE(value.toString() == "75px"); // 100 - (50/2)
    }
}
```

---

### 测试 3: 括号表达式

```cpp
TEST_CASE("Expression - 括号", "[expression]") {
    SECTION("改变优先级") {
        auto value = evaluate("(100px + 50px) * 2");
        REQUIRE(value.toString() == "300px"); // (100+50)*2
    }
    
    SECTION("嵌套括号") {
        auto value = evaluate("((100px + 50px) * 2) / 3");
        REQUIRE(value.toString() == "100px");
    }
}
```

---

### 测试 4: 单位验证

```cpp
TEST_CASE("Expression - 单位验证", "[expression]") {
    SECTION("相同单位可以加减") {
        auto value = evaluate("100px + 50px");
        REQUIRE(value.toString() == "150px");
    }
    
    SECTION("不同单位不能加减") {
        REQUIRE_THROWS_AS(
            evaluate("100px + 50em"),
            ParseError
        );
    }
    
    SECTION("无单位数字可以乘除") {
        auto value = evaluate("100px * 2");
        REQUIRE(value.toString() == "200px");
    }
}
```

---

### 测试 5: 多种单位

```cpp
TEST_CASE("Expression - 多种单位", "[expression]") {
    SECTION("em") {
        auto value = evaluate("10em + 5em");
        REQUIRE(value.toString() == "15em");
    }
    
    SECTION("rem") {
        auto value = evaluate("2rem * 3");
        REQUIRE(value.toString() == "6rem");
    }
    
    SECTION("百分比") {
        auto value = evaluate("100% - 25%");
        REQUIRE(value.toString() == "75%");
    }
}
```

---

### 测试 6: 复杂表达式

```cpp
TEST_CASE("Expression - 复杂表达式", "[expression]") {
    SECTION("多重运算") {
        auto value = evaluate("100px + 50px * 2 - 30px");
        REQUIRE(value.toString() == "170px");
    }
    
    SECTION("混合括号和优先级") {
        auto value = evaluate("(100px + 50px) * 2 - 50px / 2");
        REQUIRE(value.toString() == "275px");
    }
}
```

---

## 🔧 实现步骤

### Phase 1: CSSValue 类（1天）

**任务**:
1. 创建 `src/CHTL/CHTLCommon/CSSValue.h`
2. 实现构造函数和基本方法
3. 实现算术运算符重载
4. 实现单位验证逻辑
5. 编写单元测试

**预期结果**:
```cpp
CSSValue v1(100, "px");
CSSValue v2(50, "px");
CSSValue result = v1 + v2;
REQUIRE(result.toString() == "150px");
```

---

### Phase 2: 表达式解析器（1-2天）

**任务**:
1. 创建 `src/CHTL/CHTLParser/CSSExpression.h`
2. 实现递归下降解析
3. 处理运算符优先级
4. 支持括号
5. 错误处理

**预期结果**:
```cpp
CSSExpression expr(tokens);
CSSValue result = expr.evaluate();
```

---

### Phase 3: 集成到 Parser（0.5天）

**任务**:
1. 修改 `Parser::parseStyleBlock()`
2. 检测属性值中的表达式
3. 调用 CSSExpression 求值
4. 将结果存入 StyleNode

**预期结果**:
```chtl
style {
    width: 100px + 50px;  // Parser 自动求值为 150px
}
```

---

### Phase 4: TDD 测试（0.5天）

**任务**:
1. 创建 `tests/test_expression.cpp`
2. 编写所有测试用例
3. 确保 100% 通过
4. 更新 CMakeLists.txt

**预期结果**:
- 新增 15+ 测试用例
- 新增 50+ 断言
- 100% 通过率

---

## 📊 预期成果

### 代码统计
- **新增文件**: 3个
  - `CSSValue.h/cpp`
  - `CSSExpression.h/cpp`
  - `test_expression.cpp`

- **修改文件**: 2个
  - `Parser.cpp`
  - `tests/CMakeLists.txt`

- **新增代码**: ~500 行
  - CSSValue: ~150 行
  - CSSExpression: ~200 行
  - 测试: ~150 行

---

### 测试覆盖
- **测试用例**: 39 → **54** (+15)
- **断言数量**: 303 → **353** (+50)
- **目标通过率**: 100%

---

### 规范符合度
- **当前**: 82%
- **完成后**: 85%
- **新增规范**: CHTL.md 第 175-202 行

---

## 🎯 验收标准

### 1. 功能验收

**必须支持**:
- ✅ 基础算术运算（+, -, *, /, %, **）
- ✅ 运算符优先级
- ✅ 括号表达式
- ✅ 单位验证
- ✅ 所有 CSS 单位

**示例验收**:
```chtl
div {
    style {
        width: (100px + 50px) * 2;        // 300px
        height: 200px - 50px / 2;         // 175px
        margin: 10px * 5 + 20px;          // 70px
        padding: 100% / 4;                // 25%
    }
}
```

---

### 2. 质量验收

- ✅ 零警告编译
- ✅ 100% 测试通过
- ✅ 完整错误处理
- ✅ 符合 CHTL.md 规范

---

### 3. 性能验收

- ✅ 表达式求值 < 1ms（简单表达式）
- ✅ 内存安全（无泄漏）
- ✅ 异常安全

---

## 💡 技术难点

### 1. 运算符优先级

**挑战**: 
- `*` `/` 优先于 `+` `-`
- `**` 优先于所有

**解决**: 
- 递归下降解析（分层）
- expression → term → factor → unary → primary

---

### 2. 单位合并

**挑战**:
- `100px + 50px` → OK
- `100px + 50em` → Error
- `100px * 2` → OK

**解决**:
```cpp
bool CSSValue::canMergeWith(const CSSValue& other) const {
    // 加减：单位必须相同
    // 乘除：至少一个无单位
}
```

---

### 3. 浮点数精度

**挑战**: 
- `100px / 3` → `33.333...px`
- 显示精度问题

**解决**:
- 保留合理精度（2-4位小数）
- 去除尾随零

---

## 📚 参考资料

- CHTL.md 第 175-202 行
- CSS Values and Units Module Level 4
- Recursive Descent Parsing
- Operator Precedence

---

## 🚀 下一步（Step 5.3）

完成属性运算后，下一步将实现：

**Step 5.3: 引用属性**（CHTL.md 第 203-236 行）

```chtl
div {
    id: box;
    style {
        width: 100px;
    }
}

div {
    style {
        width: box.width + 50px;  // 引用其他元素的属性
    }
}
```

---

**预计总工作量**: 2-3 天  
**优先级**: P1（高优先级）  
**状态**: 准备开始
