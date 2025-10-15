# CHTL v0.6.0 亮点

## 🌟 三大革命性功能

### 1. 自动类名/id `.box` `#header`

**不再需要手写 class 和 id！**

```chtl
div {
    style {
        .box { width: 100px; }
    }
}
```
→ 自动生成 `class="box"`

### 2. 上下文推导 `&`

**优雅的伪类和伪元素！**

```chtl
div {
    style {
        .box { width: 100px; }
        &:hover { width: 120px; }
        &::before { content: ">"; }
    }
}
```
→ `.box:hover`, `.box::before`

### 3. 智能全局样式

**自动创建 `<head>` 和注入样式！**

```chtl
html {
    body {
        div {
            style {
                .box { width: 100px; }
            }
        }
    }
}
```
→ 自动创建 `<head>` + `<style>`

---

## 📊 数据说话

- 🚀 **+27 个测试**（+48.2%）
- 🚀 **+94 个断言**（+24.7%）
- 🚀 **+1,600 行代码**（+24.6%）
- 🚀 **+15% 规范符合度**

**83 测试 · 474 断言 · 100% 通过**

---

## ✨ 完整示例

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
                    }
                    
                    &::before {
                        content: "📝";
                    }
                }
            }
        }
    }
}
```

生成完美的 HTML + CSS！

---

**v0.6.0-style-advanced**  
**2025-10-15**
