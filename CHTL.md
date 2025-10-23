# 关于CHTL
CHTL是基于C++语言实现的超文本语言，其本质是为了提供一种更符合开发者编写HTML代码的方式，使用MIT开源协议  

## 注释
在CHTL中，使用//表示注释  
使用/**/代表多行注释  
使用# 代表会被生成器识别的注释  
//和/**/注释不会被生成器所识别，生成的HTML不会带有这些注释  
而# 注释则会根据上下文生成不同编程语言类型的注释  
生成器注释必须是# + 空格 + 注释内容  

```chtl
# 这是一个注释
#这不是一个注释
```

## 文本节点
在CHTL中，使用text { }表示一段文本

```chtl
text
{
    "这是一段文本"
}
```

除此之外，你也可以使用text属性来表示一段文本  
text属性只能在元素内部使用，而text { }可以在元素外使用  

```chtl
div
{
    text: "这是一段文本";
}
```

## 字面量
CHTL支持text与属性值使用无修饰字面量(没有引号的字符串)，这允许你像CSS一样无需书写引号  
除此之外，CHTL支持双引号字符串("")，单引号字符串('')  

```chtl
text
{
    这是一段文本
}

style
{
    color: red;
}
```

## CE对等式
CE对等式即Colon = Equal  
即在CHTL之中，':'与'='完全等价  
你可以在一些推荐的情景下使用'='  

## 元素节点
CHTL支持HTML所有的元素，无论是单标签还是双标签，还是块级，行内，行内块元素  

```chtl
html
{
    head
    {

    }

    body
    {
        div
        {
            text
            {

            }
        }

        span
        {

        }
    }
}
```

## 属性
在CHTL中，使用`属性名 : "属性值";`表示属性  

```chtl
div
{
    id: box;
    class: welcome;

    text
    {
        HelloWorld
    }
}
```

## 局部样式块
CHTL对<style></style>进行了改进，允许开发者在元素的内部嵌套style {}，以此进行一些css操作  
包括但不限于内联样式，类选择器，id选择器，伪类选择器，伪元素选择器  
类选择器，id选择器，伪类选择器，伪元素选择器相关的代码将会被自动添加至全局样式块之中  
注意！CHTL并没有对全局样式块进行增强，请不要尝试在全局样式块使用局部样式块的功能  

### 内联样式
你可以在style{}内部直接添加属性，这些属性会成为元素的内联样式  

```chtl
body
{
    div
    {
        // 内联样式
        style
        {
            width: 100px;
            height: 200px;
        }
    }
}
```

### 自动化类名 / id
你可以直接在局部样式块中使用类 / id选择器  
无需手动编写class / id  
CHTL会自动为元素添加类名 / id  

```chtl
div
{
    style
    {
        .box  // 自动添加类名box给元素，且整部分代码将自动添加至全局样式块
        {
            width: 300px;
        }
    }
}
```

### 上下文推导
你可以在局部样式块之中使用&表示类名 / id(优先类名)  
&将会根据上下文来推导成类名 / id，例如检查元素的class / id  

这里有两种使用方式，一种是上文使用class / id  
下文使用&，&:hover，&::before代替类 / id选择器  
CHTL并不建议使用这一种方式  

而是建议下述这一种使用方式  
不写元素的class / id，直接使用.box / #box，让CHTL自动添加类名 / id  
将&用于简化伪类选择器(&hover)与伪元素选择器(&::before)的使用  

```chtl
div
{
    style
    {
        .box  // 让CHTL自动添加类名
        {

        }

        &:hover  // 使用&简化伪类的使用，之后&会被解析成类名，然后整体被添加至全局样式块
        {

        }
    }
}
```

### 属性运算
在原生CSS之中，你需要使用calc()函数来进行属性运算  
在CHTL中，属性值可以直接使用算术运算符(+ - * / % **)直接进行算术运算  
其中%为取模，**为幂运算符  
全局style支持这一种用法  
注意，由于CSS带有单位，因此需要遵循同单位合并，严禁不同单位之间进行运算  

#### 运算规则详解
##### 单位合并
同单位之间进行运算，CHTL将合并单位  
不同单位之间进行运算时，CHTL将报错  

##### 左结合
无单位属性进行运算时，会寻找左侧是否存在属性值，若存在，则自动添加单位  
若左侧无属性值时，将右结合  

##### 字符串解耦
对于属性值`linear 0.5s all`，CHTL将会对此进行解耦，拿到数字字面量  
以此确保能够进行属性运算，值得一提，这种情况仅限于无修饰字面量  

##### 百分号
`1px + 5%`这样的运算是否成立？  
答案是成立，CHTL将自动将百分比转换为数字字面量，然后进行运算  

##### 特殊处理需求
`z-index: 1px + 5px`这种情况是否应该删除单位，然后得到不带单位的数字6?  
答案是不应该，这种属于开发者行为，不属于语法问题  

### 引用属性
在原生CSS之中，并不存在相对某一个属性的概念  
为此CHTL引入了指向属性  
现在你可以引用任何一个元素的属性值  
全局style不支持这一种用法  

语法为CSS选择器.属性  
处于性能的考量，只支持下述选择器  
box  自动推断，tag -> id -> class  
.box 类选择器  
#box id选择器  
button tag选择器  
.box button 后代选择器  
button[0] 精确访问  

```chtl
div
{
    id: box;
    
    style
    {
        width: 100px;
    }
}

div
{
    style
    {
        width: 100px + box.width;  // 引用box的width属性，并进行运算
    }
}
```

### 属性条件表达式  
元素自身的属性的表达式 ? 选项 : 选项  
全局style支持这一种用法  

div
{    
    style
    {
        width: 100px;
        height: 100px;
        background-color: width > 50px ? "red" : "blue";
    }
}

#### 链式调用  
div
{   
    style
    {
        width: 100px;
        height: 100px;
        background-color: 
                width > 50px ? "red" : "blue",
                width > 100px ? "green" : "yellow", 
                height < 100px ? "purple" : "pink";
    }
}

#### 可选选项  
div
{    
    style
    {
        width: 100px;
        height: 100px;
        background-color: 
                width > 50px ? "red",
                width > 100px ? "green", 
                height < 100px ? "purple" : "pink";
    }
}

#### 算术运算符
算术运算符(+ - * / % **)  
属性表达式是支持算术运算符的  
其中%为取模，**为幂运算符  

#### 逻辑运算符
逻辑与(&&)与逻辑或(||)
属性表达式允许使用逻辑与(&&)与逻辑或(||)  

```chtl
div
{   
    style
    {
        width: 100px;
        height: 100px;
        background-color: 
                width > 50px && width < 100px ? "red",
                width > 100px || height < 50px ? "green", 
                height < 100px ? "purple" : "pink";
    }
}
```

#### 引用属性条件表达式
属性条件表达式是支持参考不同元素的属性来进行条件表达的  
语法为CSS选择器.属性  
处于性能考虑，属性条件表达式只支持下述选择器  
box  自动推断，tag -> id -> class  
.box 类选择器  
#box id选择器  
button tag选择器  
.box button 后代选择器  
button[0] 精确访问  

全局style不支持这一种用法  

```chtl
div
{
    class: box;
    style
    {
        width: 100px;
    }
}

div
{
    style
    {
        width: .box.width > 50px ? .box.width / 2 : .box.height / 2;
    }
}
```

## 模板
你可以使用[Template]来创建模板，在CHTL之中，存在样式组模板，元素模板，变量组模板  
支持使用无修饰字面量  

### 样式组模板
使用`[Template] @Style 组名`来创建样式组模板  

```chtl
[Template] @Style DefaultText
{
	color: "black";
	line-height: 1.6;
}

div
{
    style
    {
        @Style DefaultText;  // 使用样式组模板，模板之中所有的属性都会被添加到div身上
    }
}
```

### 元素模板
使用`[Template] @Element 元素名`来创建元素模板  

```chtl
[Template] @Element Box
{
    span
    {
        text
        {
            这是一组div
        }
    }

    div
    {
        style
        {
            width: 200px;
            height: 200px;
            background-color: red;
        }
    }

    div
    {
        style
        {
            width: 200px;
            height: 200px;
            background-color: blue;
        }
    }
}

body
{
    @Element Box;  // 使用元素模板
}
```

### 变量组模板
使用`[Template] @Var 变量组名`来创建变量组名  

```chtl
[Template] @Var ThemeColor
{
    // 无需像CSS一样以--为前缀，因为变量组本质上不是CSS变量，而是值的替换
    tableColor: "rgb(255, 192, 203)";
}

[Template] @Element Box
{
    div
    {
        style
        {
            color: ThemeColor(tableColor);  // 使用模板变量组，无需以@Var为前缀，可以写，但不必要
        }
    }
}

body
{
    div
    {
        style
        {
            background-color: ThemeColor(tableColor);
        }
    }
}
```

### 组合继承
模板可以继承，无论什么类型，都能够继承同种类型的模板  

```chtl
[Template] @Style ThemeColor
{
    color: rgba(255, 192, 203, 1);
    background-color: rgba(253, 144, 162, 1);
}

[Template] @Style ThemeColor2
{
    background-color: yellow;  // 重复的元素，根据书写的顺序进行值的替换
    @Style ThemeColor;  // 样式组模板继承，获得ThemeColor所有的属性
}
```

#### 显性继承
除了组合式继承，你还可以使用inherit显式继承  

```chtl
[Template] @Style ThemeColor
{
    color: rgba(255, 192, 203, 1);
    background-color: rgba(253, 144, 162, 1);
}

[Template] @Style ThemeColor2
{
    background-color: yellow;
    inherit @Style ThemeColor;
}
```

## 自定义
自定义是模板的的扩展，自定义具有极高的灵活性以及更多的扩展操作  
你可以使用[Custom]创建自定义内容  
自定义与模板之间最大的差别就是自定义允许特例化操作  
模板与自定义之间允许相互继承，只要类型匹配  
支持使用无修饰字面量  

### 自定义样式组
#### 无值样式组
自定义样式组允许样式组属性不具有值，开发者需要在使用时自行填入值  

```chtl
[Custom] @Style TextSet
{
    color,
    font-size;
}

[Custom] @Style WhiteText
{
    @Style TextSet
    {
        color: white;
        font-size: 16px;
    }
}

div
{
    style
    {
        @Style TextSet
        {
            color: red;
            font-size: 16px;
        }
    }
}
```

#### 样式组的特例化
##### 删除属性
```chtl
[Template] @Style WhiteText
{
    color: white;
    font-size: 16px;
    line-height: 1.6;
    border: 1px black soild;
}

[Custom] @Style YellowText
{
    @Style WhiteText
    {
        delete line-height, border;
    }
    color: yellow;
}

div
{
    style
    {
        @Style YellowText
        {
            delete color;
        }
    }
}
```

##### 删除样式组继承
```chtl
[Template] @Style WhiteText
{
    color: white;
    font-size: 16px;
    line-height: 1.6;
    border: 1px black soild;
}

[Custom] @Style YellowText
{
    @Style WhiteText
    color: yellow;
}

div
{
    style
    {
        @Style YellowText
        {
            delete @Style WhiteText;
        }
    }
}
```

### 自定义元素
#### 自定义元素的特例化
##### 增加样式
```chtl
[Custom] @Element Box
{
    div
    {

    }

    div
    {

    }

    span
    {

    }
}

body
{
    @Element Box
    {
        span
        {
            // 为span添加样式
            style
            {

            }
        }

        div
        {

        }

        div
        {
            // 为第二个div添加样式
            style
            {

            }
        }
    }
}
```

##### 索引访问
你可以使用[index]索引来访问自定义元素中的某一个元素  

```chtl
[Custom] @Element Box
{
    div
    {

    }

    div
    {

    }

    span
    {

    }
}

body
{
    @Element Box
    {
        div[1]  // 索引访问
        {
            // 为第二个div添加样式
            style
            {

            }
        }
    }
}
```

##### 插入元素
使用`insert 位置 选择器`插入元素  

```chtl
[Custom] @Element Box
{
    div
    {

    }

    div
    {

    }

    span
    {

    }
}

body
{
    @Element Box
    {
        // 在第一个div后面插入内容
        insert after div[0] {  // 可选关键字，after，before，replace，at top / at bottom(可以不使用div[0])
            div
            {
                style
                {

                }
            }

            @Element Line;
        }
    }
}
```

##### 删除元素
```chtl
[Custom] @Element Box
{
    div
    {

    }

    div
    {

    }

    span
    {

    }

    div
    {

    }
}

body
{
    @Element Box
    {
        delete span;
        delete div[1];
    }
}
```

##### 删除元素继承
```chtl
[Custom] @Element Box
{
    div
    {

    }

    div
    {

    }

    span
    {

    }

    @Element Line;
}

body
{
    @Element Box
    {
        delete @Element Line;
    }
}
```

### 变量组
#### 变量组特例化
```chtl
[Custom] @Var ThemeColor
{
    tableColor: "rgb(255, 192, 203)";
    TextColor: "black";
}

div
{
    style
    {
        color: ThemeColor(tableColor = rgb(145, 155, 200));
    }
}
```

### 全缀名
CHTL允许使用全缀名来访问模板元素，样式组，变量组，自定义元素，样式组，变量组  
这在处理命名冲突时非常有用  

```chtl
[Custom] @Element Box
{
    div
    {

    }

    div
    {

    }
}

body
{
    @Element Box;  // 使用全缀名访问自定义元素Box

    [Custom] @Element Box;  // 使用全缀名访问自定义元素Box
}
```

## 原始嵌入
在CHTL中，你可以使用[Origin]表示这是一段原始的代码，这部分代码不会被CHTL处理，而是让生成器直接生成  
原始嵌入是CHTL的兼容处理机制，避免CHTL考虑不到的极端问题  
原始嵌入允许在任意节点中被解析  
原始嵌入是直接把内容进行输出，绝对不会进行处理  
原始嵌入的类型无作用，仅提供标识，不要根据类型将其包装在不同的标签中  

### 嵌入HTML代码
```chtl
[Origin] @Html
{

}

body
{
    [Origin] @Html
    {

    }
}
```

### 嵌入CSS代码
```chtl
[Origin] @Style
{

}
```

### 嵌入JS代码
```chtl
[Origin] @JavaScript
{

}
```

### 带名原始嵌入
你可以为原始嵌入块赋予一个名称  
```chtl
[Origin] @Html box
{

}

body
{
    [Origin] @Html box;
}
```

## 导入
你可以使用[Import]导入CHTL，HTML，CSS，JS文件  
路径支持无修饰字面量  

### 导入HTML，CSS，JS文件
导入HTML文件  
[Import] @Html from html文件路径 as(必须) 命名为  

导入CSS文件  
[Import] @Style from css文件路径 as(必须) 命名为  

导入JS / CJJS文件  
[Import] @JavaScript from js文件路径 as(必须) 命名为  // 如果不具有as，直接跳过，具有as，则是创建命名原始嵌入节点  

对于上述三种类型  
如果写的是文件名（不带后缀）：在编译文件所在目录（非递归）按类型搜索相关文件  
具体文件名（带后缀）：在编译文件所在目录（非递归）直接搜索该文件  
如果路径为文件夹或不包含具体文件信息时，触发报错  

### 导入CHTL文件
#### 精确导入

导入另一个chtl文件之中的自定义元素  
[Import] [Custom] @Element 需要导入的自定义元素名 from chtl文件路径 as(可选) 命名为  

导入另一个chtl文件之中的自定义样式组  
[Import] [Custom] @Style 需要导入的样式组名称 from chtl文件路径 as(可选) 命名为  

导入另一个chtl文件之中的自定义变量组  
[Import] [Custom] @Var 需要导入的变量组名称 from chtl文件路径 as(可选) 命名为  

导入另一个chtl文件之中的元素模板  
[Import] [Template] @Element 需要导入的自定义元素名 from chtl文件路径 as(可选) 命名为  

导入另一个chtl文件之中的样式组模板  
[Import] [Template] @Style 需要导入的样式组名称 from chtl文件路径 as(可选) 命名为  

导入另一个chtl文件之中的变量组模板  
[Import] [Template] @Var 需要导入的变量组名称 from chtl文件路径 as(可选) 命名为  

导入另一个chtl文件之中的命名原始嵌入  
[Import] [Origin] @Html 名称 from chtl文件路径 as(可选) 命名为  
[Import] [Origin] @Style 名称 from chtl文件路径 as(可选) 命名为  
[Import] [Origin] @JavaScript 名称 from chtl文件路径 as(可选) 命名为  

#### 类型导入
导入另一个chtl文件之中所有的自定义元素  
[Import] [Custom] @Element from chtl文件路径 as(无效) 命名为  

导入另一个chtl文件之中所有的自定义样式组  
[Import] [Custom] @Style from chtl文件路径 as(无效) 命名为   

导入另一个chtl文件之中所有的自定义变量组  
[Import] [Custom] @Var from chtl文件路径 as(无效) 命名为   

导入另一个chtl文件之中所有的元素模板  
[Import] [Template] @Element from chtl文件路径 as(无效) 命名为  

导入另一个chtl文件之中所有的样式组模板  
[Import] [Template] @Style from chtl文件路径 as(无效) 命名为  

导入另一个chtl文件之中所有的变量组模板  
[Import] [Template] @Var from chtl文件路径 as(无效) 命名为  

#### 通配导入
导入所有模板  
[Import] [Template] from chtl文件路径 as(无效) 命名为  

导入所有自定义  
[Import] [Custom] from chtl文件路径 as(无效) 命名为  

导入所有命名原始嵌入(注意！命名)  
[Import] [Origin] from chtl文件路径 as(无效) 命名为  

导入一个chtl文件  
[Import] @Chtl from chtl文件路径  

#### . /对等式
在CHTL，路径具有两种表达方式，可以使用'.'来表示'/'  

## 命名空间
你可以使用[Namespace]创建命名空间，命名空间能够有效防止模块污染  
导入一整个文件，或导入了重名的任意单元时，命名空间起效  
命名空间允许不使用{}  

test.chtl  
```chtl
[Namespace] space  // 创建space命名空间

[Custom] @Element Box
{
    div
    {
        style
        {

        }
    }

    div
    {
        style
        {

        }
    }
}
```

```chtl
[Import] @Chtl from test.chtl的路径  // 导入一个chtl文件时，如果对方没有定义命名空间，则默认使用文件名作为命名空间

body
{
    @Element Box from space;  // 使用space命名空间中的Box自定义元素
}
```

通常情况下，from是非必要使用的，这是因为CHTL使用的是一种伪合并的机制，让多个命名空间共享上下文  
只有在必要的使用，才需要使用from  

### 命名空间嵌套
```chtl
[Namespace] space
{
    [Namespace] room  // 嵌套命名空间

    [Custom] @Element Box
    {
        div
        {
            style
            {

            }
        }

        div
        {
            style
            {

            }
        }
    }
}
```

```chtl
[Namespace] space
{
    [Namespace] room


    [Namespace] room2
    
        [Custom] @Element Box
        {
            div
            {
                style
                {

                }
            }

            div
            {
                style
                {

                }
            }
        }
}
```

```chtl
[Import] @Chtl from test.chtl的路径

body
{
    @Element Box from space.room2;  // 嵌套命名空间的使用
}
```

同名的命名空间自动合并，并且具有冲突检测策略  
对于没有使用命名空间的文件，在被导入时，会默认以文件名作为命名空间  

## 约束
你可以使用except关键字来进行定义域约束  

### 精确约束
精确约束能够作用的对象有HTML元素，自定义与模板对象  

```chtl
div
{
    except span, [Custom] @Element Box;  // 禁止使用span与Box
}
```

### 类型约束
精确约束能够作用的对象有@Html，[Custom]，以及模板[Template]  

```chtl
div
{
    except @Html;  // 禁止html元素
    except [Template] @Var; // 禁止在div内部使用变量组模板的对象
    except [Custom];  // 禁止出现[Custom]类型的对象
}
```

### 全局约束
在命名空间内使用全局约束，全局约束只支持前面列出来的类型  

```chtl
[Namespace] space
{
    except [Template];

    [Template] // 错误，不允许定义模板
}
```

## 配置组
配置组允许开发者自定义很多行为  
配置组支持使用无修饰字面量  

```chtl
[Configuration]
{
    // 索引的起始计数
    INDEX_INITIAL_COUNT = 0;

    // DEBUG模式
    DEBUG_MODE = false;
}
```

### Name
你可以通过修改配置组中的[Name]块来修改关键字名称  

```chtl
[Configuration]
{
    INDEX_INITIAL_COUNT = 0;

    // 是否禁用Name配置组(即是否允许自定义关键字名称)
    DISABLE_NAME_GROUP = true;

    DEBUG_MODE = false;

    // Name配置块，存放关键字名称用
    [Name]
    {
        // 这里可以定义关键字的名称，无需改动源码即可实现支持
        // 组选项，即CUSTOM_STYLE具有多个值
        CUSTOM_STYLE = [@Style, @style, @CSS, @Css, @css];
        CUSTOM_ELEMENT = @Element;
        CUSTOM_VAR = @Var;
        TEMPLATE_STYLE = @Style;
        TEMPLATE_ELEMENT = @Element;
        TEMPLATE_VAR = @Var;
        ORIGIN_HTML = @Html;
        ORIGIN_STYLE = @Style;
        ORIGIN_JAVASCRIPT = @JavaScript;
        IMPORT_HTML = @Html;
        IMPORT_STYLE = @Style;
        IMPORT_JAVASCRIPT = @JavaScript;
        IMPORT_CHTL = @Chtl;
        IMPORT_CRMOD = @CJmod;
        KEYWORD_INHERIT = inherit;
        KEYWORD_DELETE = delete;
        KEYWORD_INSERT = insert;
        KEYWORD_AFTER = after;
        KEYWORD_BEFORE = before;
        KEYWORD_REPLACE = replace;
        KEYWORD_ATTOP = at top;
        KEYWORD_ATBOTTOM = at bottom;
        KEYWORD_FROM = from;
        KEYWORD_AS = as;
        KEYWORD_EXCEPT = except;
        KEYWORD_USE = use;
        KEYWORD_HTML5 = html5;
        KEYWORD_TEXT = text;
        KEYWORD_STYLE = style;  // 局部样式块
        KEYWORD_SCRIPT = script;  // 局部脚本块
        KEYWORD_CUSTOM = [Custom];
        KEYWORD_TEMPLATE = [Template];
        KEYWORD_ORIGIN = [Origin];
        KEYWORD_IMPORT = [Import]
        KEYWORD_NAMESPACE = [Namespace]

        // 组选项的数量限制，避免在大型项目中对性能的过高消耗
        OPTION_COUNT = 3;
    }
}
```

### 命名配置组
配置组可以命名，命名配置组不会被使用，不被命名的配置组才会启用，如果存在多个无名的配置组则冲突，命名配置组可以创建多个，命名配置组通常服务于导入[Import]  

```chtl
[Configuration] @Config Basic
{
    [Name]
    {
        CONFIGURATION_CONFIG = @Config;  // 新增
        IMPORT_CONFIG = @Config;  // 新增
    }
}

[Configuration] @Config Std
{
    
}
```

### 导入配置组
[Import] @Config 导入的配置组名称(可选，不写默认导入无名配置组，如果没有则是第一个有名) from chtl文件路径 as(如果使用as，需要与use搭配使用) 命名为  

全缀名写法  
[Import] [Configuration] @Config 导入的配置组名称(可选，不写默认导入无名配置组，如果没有则是第一个有名) from chtl文件路径 as(如果使用as，需要与use搭配使用) 命名为  

导入另一个Chtl文件之中所有的命名配置组  
[Import] [Configuration] from chtl文件路径 as(无效) 命名为  

### 局部样式块自动化规则
你可以在[Configuration]之中添加下述规则禁用局部样式块class / id的自动化添加  

```chtl
[Configuration]
{
    // 禁止局部样式块自动添加类选择器
    DISABLE_STYLE_AUTO_ADD_CLASS = false;
    // 禁止局部样式块自动添加id选择器
    DISABLE_STYLE_AUTO_ADD_ID = false;
}
```

当局部style内部存在多组类选择器时，若class属性缺失，则自动添加第一个类选择器  
当局部style内部存在多组id选择器时，若id属性缺失，则自动添加第一个id选择器  
对于局部style来说，& 引用选择器优先选择class  

### 默认命名空间
你可以在[Configuration]之中添加下述规则禁用默认命名空间功能，这意味着导入的文件与当前文件所使用的命名空间一致，这可能会造成污染  

```chtl
[Configuration]
{
    DISABLE_DEFAULT_NAMESPACE = false;
}
```

### 自定义原始嵌入类型
@Html，@Style，@JavaScript是CHTL中基本的类型，CHTL只为原始嵌入提供了这三种类型  
如果你需要更多的类型，你可以直接创建你想要的类型的原始嵌入，注意！必须以@为前缀    

```chtl
[Origin] @Vue box
{

}

body
{
    [Origin] @Vue box;
}
```

自定义类型系统会隐式创建一个配置块  

```chtl
[Configuration]
{
    DISABLE_CUSTOM_ORIGIN_TYPE = false;

    [OriginType]  // 定义Origin具有什么自定义类型
    {
        ORIGINTYPE_VUE = @Vue;  // 如果我创建了一个@Vue类型的原始嵌入，那么CHTL会隐式创建

        // 你也可以显式创建，更明确说明，但是要注意  
        // ORIGINTYPE_VUE = @Vue;
        // ORIGINTYPE_MARKDOWN = @Markdown;
        // CHTL强制要求使用ORIGINTYPE_全写的类型名称 = @全大写后 = 全写的类型名称的内容
    }
}
```

#### 导入自定义原始嵌入类型
// 导入另一个chtl文件之中某一个命名自定义原始嵌入
[Import] [Origin] @Vue 原始嵌入名称 from xx.chtl as(可选) 命名  

// 导入另一个chtl文件之中所有的命名自定义原始嵌入
[Import] [Origin] @Vue from xx.chtl as(无效) 命名  

## use
use语法能够明确当前文件使用什么配置组  
use语法必须在文件开头，且只能有一个用于配置组  

### HTML5类型
```chtl
use html5;
```

生成HTML5声明  

### 使用命名配置组
```chtl
use @Config Basic;  // 此文件使用Basic配置组

use [Configuration] @Config Basic;  // 也可以使用全缀名
```

## 职权
CHTL不具有处理JS的能力，CHTL只服务于HTML + CSS，不服务于JS，不会处理那些具有动态特征的语法  
这些动态特征皆通过盐桥，CHTL JS编译器等组件，让CHTL JS去处理  
盐桥是CHTL与CHTL JS之间的通信桥梁  

通常情况下，script块由CHTL JS编译器解析，必要时会通过盐桥请求CHTL编译器，反之CHTL编译器也会通过盐桥请求CHTL JS编译器  

## CHTL JS
CHTL JS是CHTL项目的扩展语法(不是CHTL的JS，而是CHTL项目能够使用这一门编程语言)，并不是JS的超集，也不支持JS的语法  
CHTL JS完全独立于JS，是一门独立的编程语言，与JS毫无关系，只是最终转变为JS代码  
JS的语法由CHTL内置的JS编译器解析，CHTL JS的语法由CHTL JS编译器解析  
两者之间并不兼容，CHTL JS的语法是CHTL JS编译器的扩展语法  
这些是你正确实现CHTL JS的基础，请不要试图让CHTL JS去解析JS代码  

CHTL JS使用独立的编译器  

CHTL JS的函数皆为声明式语法  
均支持无序键值对，可选键值对，无修饰字面量  

### 文件后缀
对于包含CHTL JS的JS文件，你可以命名为*.cjjs  

### ScriptMain
ScriptMain是CHTL JS的入口，只能用于编写纯CHTL JS语法，不兼容JS语法  
这是早期阶段的一种妥协，是实现实现混合CHTL JS和JS支持前的一种选择  
此功能将一直保留，将用于那些希望独立使用CHTL JS的开发者  

ScriptMain会直接传递那些JS代码，仅仅只处理纯CHTL JS代码  
只有CHTL项目真正实现混合语法处理后，ScriptMain将被标记为弃用  

```
ScriptMain
{
    {{box}}->Listen {
        click: () => {

        },

        mouseenter: mouseEnterEvent,  // 已经存在的函数

        mousemove: function() {

        }
    };
}
```

### 脚本加载器
你可以使用ScriptLoader {}来导入文件  
CHTL JS实现了AMD风格JavaScript文件加载器  
目的是让开发者能够无序引入js文件，无需考虑加载顺序，文件依赖等问题  
支持无序键值对，可选键值对，无修饰字面量  

```chtl
你可以使用ScriptLoader {
    load: ./module.cjjs,
    load: ./module2.cjjs,
    load: ./module3.cjjs,
    load: ./module4.cjjs
    load: ./module5.js  // 普通js文件也可以  

    load:
    ./module.cjjs
    ,./module2.cjjs
    ,./module3.cjjs
    ,./module4.cjjs
    ,./module5.js  // 也支持链式语法
}
```

### 局部script
CHTL允许在局部样式块中使用script{}来编写JS代码  
局部script会被添加到一个不会全局污染，具有高优先级的全局script块之中  

注：局部script属于CHTL，但是所有动态特征都属于CHTL JS，script块全权交给CHTL JS处理    

```chtl
div
{
    style
    {
        .box
        {
            width: 100px;
            height: 100px;
            background-color: red;
        }
    }

    script
    {
        {{box}}.addEventListener('click', () => {
            console.log('Box clicked!');
        });

        // 引用功能
        {{&}}->addEventListener('click', () => {
            console.log('Box clicked!');
        });
    }
}
```

### 增强选择器
你可以使用{{CSS选择器}}来创建一个DOM对象  
支持无修饰字面量  

```chtl
button
{
    style
    {
        .box
        {
            
        }
    }
}

script
{
    {{box}}.textContent()  // 先判断是否为tag，然后查找类名 / id = box(id优先)的元素，并创建DOM对象
    {{.box}}  // 查找类名为box的元素，并创建DOM对象
    {{#box}}  // 查找id为box的元素，并创建DOM对象
    {{button}}  // 所有的button元素
    {{.box button}}  // 查找类名为box的元素的所有的button后代，并创建DOM对象  
    
    // 精确访问
    {{button[0]}}  // 第一个button元素

    // 增强选择器仅支持上述的种类，这是出于性能与复杂性之间的考虑
    // .boxbutton这种交集选择器会消耗很多性能，因此这里不得不放弃支持
    // 增强选择器不同于CSS属性条件表达式的选择器(因为那个不需要解析)，无法做到多类型支持  
}
```

#### 自动化规则
你可以在[Configuration]之中添加下述规则禁用自动化    

```chtl
[Configuration]
{
    // 禁止局部脚本自动添加类选择器
    DISABLE_SCRIPT_AUTO_ADD_CLASS = true;
    // 禁止局部脚本自动添加id选择器
    DISABLE_SCRIPT_AUTO_ADD_ID = true;
}
```
如果DISABLE_SCRIPT_AUTO_ADD_CLASS 和 DISABLE_SCRIPT_AUTO_ADD_ID 为真  
当局部script内部存在多组类选择器时，若class属性，局部style没有触发class自动化添加缺失，第一个{{.box}}会被自动添加  
当局部script内部存在多组id选择器时，若id属性，局部style没有触发id自动化添加缺失，第一个{{#box}}会被自动添加  
{{box}}不会自动添加，只有{{.box}}和{{#box}}能够触发自动化添加  
对于局部script来说，& 引用选择器优先选择id  

### 明确使用CHTL语法
使用到CHTL JS语法时，我们推荐使用->代替.  
以便明确使用了CHTL JS语法  
->与.是完全等价的，因此你可以直接使用->进行链式访问  

```chtl
button
{
    style
    {
        .box
        {
            
        }
    }
}

script
{
    {{box}}->textContent();
}
```

### 增强监听器
你现在可以使用Listen来快捷绑定事件监听器  
支持无序键值对，可选键值对，无修饰字面量  

```chtl
button
{
    style
    {
        .box
        {

        }
    }
}

script
{
    // 声明式
    {{box}}->Listen {
        click: () => {

        },

        mouseenter: mouseEnterEvent,  // 已经存在的函数

        mousemove: function() {

        }
    };
}
```

### 事件绑定操作符
你现在可以使用 &-> 快捷为一个DOM对象绑定事件  
支持无序键值对，可选键值对，无修饰字面量  

```chtl
{{box}} &-> click: () => {};  
```

#### 多事件绑定
你可以使用 , 分隔多个事件，同时为多个事件绑定同一个函数  

```chtl
{{box}} &-> click, mouseenter, mouseleave: () => {};
```

#### 链式绑定
事件绑定操作符支持链式绑定，注意使用 , 分隔  

```chtl
{{box}} &-> click: () => {},
        &-> mouseenter: () => {},
        &-> mouseleave: () => {};
```

#### 绑定块
如果上述用法不适合你，你可以使用绑定块  

```chtl
{{box}} &-> {
    click: () => {},
    mouseenter: () => {},
    mouseleave: () => {}
}
```

### 事件委托
为了解决SPA页面中元素动态更新导致事件监听丢失的问题，提供了基于事件委托的增强语法  
通过将事件绑定到不会销毁的父元素，监听冒泡阶段的事件，从而实现稳定的事件绑定  
支持无序键值对，可选键值对，无修饰字面量  

```chtl
script
{
    {{父元素选择器}}->Delegate {
        target: {{选择器}} | [{{选择器1}}, {{选择器2}},...], // 要代理的子元素对象 / 子元素对象数组
        click: 函数,  // 绑定的事件类型及对应回调函数
        mouseenter: 函数,
        mouseleave: 函数,
    };
}
```
需要创建一个全局注册表，管理所有事件委托的父元素，重复绑定父元素的子元素会作为分支合并在同一个事件委托之中  
避免创建多个相同的事件委托  

### 动画
CHTL JS简化了动画的使用，封装了requestAnimationFrame  
支持无序键值对，可选键值对，无修饰字面量  

```chtl
script
{
    const anim = Animate {
        target: {{选择器}} || [{{选择器1}}, {{选择器2}}] || DOM对象
        duration: 100,  // 动画持续时间，ms
        easing: ease-in-out,  // 缓慢函数

        begin: {  // 起始状态，写css代码

        }

        when: [
            {
                at: 0.4;  // 动画播放到什么时刻

                // css代码
                opacity: 0.5,
                transform: 'scale(1.1)'
            },
            {
                at: 0.8;  // 动画播放到什么时刻
                // css代码
            }
        ]

        end: {  // 终止状态，写css代码

        }

        loop: -1,  // 循环次数
        direction: ,  // 播放的方向
        delay:  ,  // 开始播放的延迟，ms
        callback: function,  // 播放完毕后做什么
    };
}
```

### 虚对象
虚对象是CHTL JS重要的特征之一，虚对象提供了访问CHTL JS函数的元信息能力  
虚对象能够获取CHTL JS函数的任意键的键值  
支持无序键值对，可选键值对，无修饰字面量  

```
Vir test = Listen {
    click: () => {

    }

    other: {

    }
};

test->click();  // 解析click为函数引用
test->other;  // 解析other为对象
```

Vir是CHTL JS层面的语法糖，不涉及JS  
Listen会按原样生成JS代码  
Vir本身就不存在，是编译期间的语法糖  

```
Vir Test = Listen {
    click: ()=>{

    }
};
```
编译器扫描到Vir时，会创建一个C++对象，这个C++对象负责Vir的解析  
假设这个对象为View  
View对象需要做两件事情，一件是记录Vir虚对象的名称，第二个是解析CHTL JS函数中的键，并创建对应表  
后续在解析时，遇到Test->click;时，会根据键值的类型，转换成不同的内容，比如函数引用，对象，数组等，并且这些结果会缓存在View之中，以便后续的解析  

### 路由
你可以使用路由快速创建SPA页面的基本架构  
支持无序键值对，可选键值对，无修饰字面量  

```chtl
Router {  // 1对1的关系
    url: "/home",
    page: {{选择器}},
}

Router {  // 1对1的关系
    url: "/home", "/about"
    page: {{选择器1}}, {{选择器2}}
}
```

```chtl
Router {  // 1对1的关系，对象形式
    page: {"/home", {{选择器}}}
        , {"/about",{{选择器2}}}
}

Router {  // 设置根路径 / 设置根容器
    root: "/" || {{选择器}},  // 默认根路径 / 变化元素的根容器，这是二选一形式
}

Router {  // 设置根路径的同时设置根容器
    root: {"/", {{选择器}}},  // 也可以是全选，对象的形式
}

Router {  // 模式
    mode: "history" || "hash",  // 路由的模式
}
```

### 动态属性条件表达式
无论是属性条件表达式，还是引用属性条件表达式，其本质都是静态的  
而CHTL JS提供了动态的属性条件表达式  
全局style不支持这一种用法  

```chtl
div
{
    id: box;
    style
    {
        width: 100px;
    }
}

div
{
    style
    {
        width: {{box}}->width > 2 ? 100px : 50px;  // 动态变化
    }
}
```

### 响应式值
这是CHTL与CHTL JS的首次交互尝试  
响应式值属于CHTL JS的范畴，但是与CHTL交互  
语法：$JS变量名$  

```chtl
div
{
    class = $boxClass$;  // 根据JS变量来设置值

    style
    {
        width: $boxWidth$ + 20px;  // 根据JS变量来设置值，算术运算自然是支持的
    }

    script
    {
        let boxClass = "box";  // 绑定的JS变量
        let boxWidth = 100;  // 绑定的JS变量

        {{boxClass}}.textContent = "Hello, CHTL!";
    }
}
```
同样，全部样式块因为缺少层级概念，为此响应式值不支持全局样式块  

## 模块
### 模块路径
什么是模块路径？  
模块路径就是CHTL编译器存放模块源码 / 模块搜索等功能的路径  

主要有如下规则  
源码文件夹 -> Module文件夹  
官方模块目录 -> 编译器二进制文件所在目录的module文件夹  
用户模块目录  ->  编译文件所在目录的module文件夹  

上述的文件夹都支持两种结构，无序结构和有序结构  
无序结构：cmod，chtl，cjmod文件混杂在一起，不分文件夹  
有序结构：使用cmod / Cmod / CMOD + cjmod / CJmod / CJMOD两个文件夹进行分类  

### CMOD
CMOD是CHTL提供的一种模块化方式  
CHTL编译器 / 打包脚本能够将符合CMOD格式的文件夹打包成.cmod文件  
你需要获取CHTL编译器的源码，CHTL源码中包含了打包脚本，如果你会编译CHTL编译器，那么更推荐你使用CHTL编译器来打包CMOD  

#### 模块结构
CMOD具有严格的模块结构  

```chtl
Chtholly  
    src  
        Chtholly.chtl  
        Other.chtl  
    info  
        Chtholly.chtl  
```

src是模块的源码，而info则是存放模块信息的文件夹  
CHTL要求，模块文件夹，主模块chtl文件，模块信息chtl文件必须同名  

在没有子模块的情况下，主模块chtl文件必须存在  

如果src中存在子模块，则src文件夹内部的主模块chtl文件可以省略(推荐)，也可以保留，保留的意义并不大  
最好是让主模块作为一个框架  

#### 模块信息
info文件夹中的chtl文件主要提供两个功能  
提供模块的信息与提供外部查询表  

如下所示  
```chtl
// 只需要写这个即可
[Info]
{
    name = "chtholly";
    version = "1.0.0";
    description = "珂朵莉主题模块 - 世界上最幸福的女孩";
    author = "CHTL Team";
    license = "MIT";
    dependencies = "";
    category = "theme";
    minCHTLVersion = "1.0.0";
    maxCHTLVersion = "2.0.0";
}

// 外部查询表，优化性能用，同时决定向外暴露什么内容
// 不需要手动写这个，系统自动生成，也可以手动书写，决定向外暴露什么内容
[Export]
{
    [Custom] @Style ChthollyStyle, ChthollyCard, ChthollyButton,
           ChthollyHeader, ChthollyFooter;

    [Custom] @Element ChthollyPage, ChthollySection, ChthollyNav,
             ChthollyModal, ChthollyGallery;

    [Custom] @Var ChthollyColors, ChthollyFonts, ChthollySpacing,
         ChthollyBreakpoints, ChthollyAnimations;

    [Template] @Style ...;
    [Template] @Element ...;
    [Template] @Var ...;
    [Origin] @Html ...;
    [Origin] @Style ...;
    [Origin] @Javascript ...;
    [Origin] @Vue ...;  // 一样支持导出自定义类型的原始嵌入
    [Configuration] @Config ...;
}
```

#### 包含子模块的模块结构
```chtl
Chtholly  
    src  
        (Chtholly.chtl)  // 存在子模块，可选
        Other  
            src  
                Other.chtl
            info  
                Other.chtl
        Space  
            src  
                Space.chtl
            info  
                Space.chtl
    info  
        Chtholly.chtl  
```
拥有子模块的模块文件夹内部主模块chtl文件可以省略，也可以保留，保留的意义不大  
但是必须注意的是，结构始终是src + info，这是不可忽略的  
主模块chtl文件即使省略了也必须拥有info  
相应的子模块均使用src + info的结构  
这是强制的要求  

#### 模块的导入
导入一个chtl文件  /  cmod模块  
[Import] @Chtl from chtl / cmod文件路径  

##### 路径搜索
对于@Chtl类型来说  
名称（不带后缀）：优先搜索官方模块目录(源码编译后生成的module文件夹，通常和编译器同一个文件夹，含cmod，chtl和cjmod文件，module文件夹可能分为两种情况，一种是乱序结构，cmod，chtl，cjmod的文件混杂在一起，一种是有序结构，使用cmod / Cmod / CMOD + cjmod / CJmod / CJMOD两个文件夹进行分类），其次搜索编译文件所在的目录module(module文件夹可能分为两种情况，一种是乱序结构，cmod，chtl，cjmod的文件混杂在一起，一种是有序结构，使用cmod / Cmod / CMOD + cjmod / CJmod / CJMOD两个文件夹进行分类)文件夹，最后搜索编译文件所在目录，优先匹配cmod文件，其次chtl，不匹配cjmod文件)
具体名称（带后缀）：按官方模块目录→当前目录module文件夹→当前目录顺序搜索指定文件
具体路径（含文件信息）：直接按路径查找，未找到则报错
具体路径（不含文件信息）：触发报错
对于使用官方模块前缀，直接在官方模块目录中搜索  

### CJMOD
CJMOD是CHTL JS提供一种模块化方式，开发者可以使用CHTL JS提供的CJMOD API来实现CHTL JS语法  
CJMOD让扩展CHTL JS语法变得更简单，无需阅读源码即可实现CHTL JS语法的扩展，并通过CJMOD的方式发布    

#### 模块结构
CJmod具有严格的模块结构  
对于没有子模块的CJMOD，通常结构如下，CJMOD无需像CJMOD一样，需要模块文件夹名称，主模块文件，信息文件三者同名  
仅仅需要模块文件夹与信息文件两者同名即可  

```chtl
CJmod文件夹  
    src/xxx.cpp xxx.h  
    info/CJmod文件夹名称.chtl  
```

#### 模块信息
与CMOD不同，CJMOD的info文件无法使用[Export]，这源于CJMOD的特殊性  
CJMOD只需要使用[Info]即可  

```chtl
[Info]
{
    name = "chtholly";
    version = "1.0.0";
    description = "珂朵莉主题模块 - 世界上最幸福的女孩";
    author = "CHTL Team";
    license = "MIT";
    dependencies = "";
    category = "theme";
    minCHTLVersion = "1.0.0";
    maxCHTLVersion = "2.0.0";
}
```

#### 包含子模块的模块结构
```chtl
CJmod文件夹  
    src/xxx.cpp xxx.h  
        Box  
            src/xxx.cpp xxx.h  
            info/Box.chtl  
        Box2  
            src/xxx.cpp xxx.h  
            info/Box2.chtl  
    info/CJmod文件夹名称.chtl  
```
对于包含子模块的模块结构，仅且只能像上述演示的结构一样创建  
主文件直接位于src之下，其他内容则位于相关的模块文件夹内部  
同样，CJMOD也严格使用src + info的结构  

#### 模块的导入
导入CJmod文件  
[Import] @CJmod from cjmod文件路径  

对于@CJmod类型来说  
名称（不带后缀）：优先搜索官方模块目录，其次搜索当前目录module文件夹，最后搜索当前目录，仅匹配cjmod文件  
具体名称（带后缀）：按官方模块目录→当前目录module文件夹→当前目录顺序搜索指定文件  
具体路径（含文件信息）：直接按路径查找，未找到则报错  
具体路径（不含文件信息）：触发报错
对于使用官方模块前缀，直接在官方模块目录中搜索  

### CMOD + CJMOD
如果你想要提供组件的同时提供扩展CHTL JS语法  
你可以使用CMOD + CJMOD的混合模块结构  
CMOD + CJMOD的混合模块始终使用src + info的结构  

注：CMOD + CJMOD的混合模块结构最终得到的是CMOD模块文件  

#### 模块结构
```chtl
模块名称  
    CMOD / Cmod / cmod  
        src/
            (模块名称.chtl)  // -> 主模块文件，子模块存在时可选
            (xxx.chtl)  // -> 其他模块文件
            Box  // ->子模块
                src/Box.chtl, Other.chtl  
                info/Box.chtl  
            Box
        info/模块名称.chtl
    CJMOD / CJmod / cjmod  
        src/xxx.cpp xxx.h  // -> 主模块文件，必须
            Box  // ->子模块
                src/xxx.cpp xxx.h  
                info/Box.chtl  
        info/模块名称.chtl
```
注意，CMOD和CJMOD并不共用信息文件，你可以吧CMOD + CJMOD的混合模块结构，视为一个module文件夹，内部存放着标准的cmod和cjmod模块  
CMOD和CJMOD文件夹内部实际使用的是标准的CMOD和CJMOD的组织方式  
由于信息文件的不共用，你可以自由控制CMOD和CJMOD的版本，允许他们在不同的版本之中兼容  

如何使用？例如这个模块叫Box，那么如果我想要调用Box的CMOD模块时，我们直接使用[Import] @Chtl即可  
如果需要使用CJMOD，需要使用[Import] @CJmod，CHTL不会对此进行统一处理，我们不推荐使用@Chtl同时管理CMOD和CJMOD  

### 通配符
[Import] @Chtl from 具体路径.*  // 导入具体路径下的所有.cmod和.chtl文件  
[Import] @Chtl from 具体路径.*.cmod  // 导入具体路径下的所有.cmod文件  
[Import] @Chtl from 具体路径.*.chtl  // 导入具体路径下的所有.chtl文件  
等价于
[Import] @Chtl from 具体路径/*  // 导入具体路径下的所有.cmod和.chtl文件  
[Import] @Chtl from 具体路径/*.cmod  // 导入具体路径下的所有.cmod文件  
[Import] @Chtl from 具体路径/*.chtl  // 导入具体路径下的所有.chtl文件  

// 导入子模块时，支持使用'/'替代'.'作为路径分隔符，因为在CHTL中，'.'和'/'等价  
[Import] @Chtl from Chtholly.*  // 导入Chtholly模块的所有子模块  
[Import] @Chtl from Chtholly.Space  // 导入Chtholly模块中指定的Space子模块  

### 官方模块
#### 官方模块前缀
你可以使用"chtl::"明确表明使用官方模块  
[Import] @Chtl from chtl::Chtholly  // 导入官方模块Chtholly  

#### Chtholly 珂朵莉模块
珂朵莉对于我来说，是一个很特别的角色，是我一直喜欢着的人物，我希望我能让珂朵莉成为CHTL中重要的模块  
珂朵莉模块采用CMOD + CJMOD的混合模块  

##### CMOD
###### 手风琴
###### 四叶窗相册
###### 备忘录
###### 暖色笔记
###### 樱花雨
###### 鼠标特效
###### 鼠标拖尾
###### 视差滚动背景
###### 右键菜单栏
###### 进度条

##### CJMOD
###### PrintMylove
PrintMylove可以将一张图片变成字符像素块的形式，你可以使用PrintMylove来把图片转换成字符像素块或ASCII的形式  
然后输出到控制台  
支持无序键值对，可选键值对，无修饰字面量  
```chtl
const str = PrintMylove {
    url: ,
    mode: ,  // 模式可以选择ASCII或Pixel
    width: ,  // 宽度，支持的单位有CSS单位以及百分比，小数，纯数字(像素)
    height: ,  // 高度
    scale:  ,  // 缩放倍数，限定为等比缩放策略
};
```

###### INeverAway
INeverAway是一个很特别的功能，从名称上面你完全是理解不到这个功能的实际作用的 INeverAway用于创建一组标记函数  
INeverAway与其他CHTL JS功能不同，它允许开发者定义键，而不是使用键，并可以使用状态区分同名的键  
INeverAway需要与虚对象共用  
支持无序键值对，可选键值对，无修饰字面量  

```chtl
Vir Test = INeverAway {
    Void<A>: function(int, int) {

    },

    Void<B>: funtion(int, int) {  // 通过状态同名同参重载

    },

    Void: {

    },

    Ax: {

    }
};

Test->Void<A>();
```
Test是虚拟对象，是不存在的对象，这里并没有创建一个对象  

实现原理：  
INeverAway  ->  创建一组JS全局函数，名称由CHTL编译器统一管理，在调用时才生成对应的JS函数代码  
Vir对象本身不存在，最终转变成相对应的函数的引用  

INeverAway函数存在的意义其实很迷惑人，这是因为相对于使用INeverAway，更多人更喜欢使用普通的函数  
这是当然，毕竟INeverAway存在的意义本身就不是作为实用功能而存在，然而，INeverAway其实是CHTL JS的一种新方向  
也是对函数重载的重定义  

###### util...end表达式

util 表达式 -> change { 条件变化时 } -> end { 条件完成时 }

util a > b -> change print("发生变化") -> end print("a > b");  // 单行语句情况下，change条件可以不写分号
util a > b -> change print("发生变化"); -> end print("a > b");
util a < b -> change {print("发生变化");} -> end {print("a < b");}  // 多行代码下，无论如何都要以分号结束

#### Yuigahama 由比滨结衣模块
由比滨结衣模块使用CMOD

##### 音乐播放器
##### 手风琴
##### 四叶窗相册
##### 备忘录
##### 暖色笔记
##### 樱花雨
##### 鼠标特效

编译生成的module文件夹(官方模块)有两种结构，一种就是常规的混杂，chtl文件，cmod，cjmod  
一种是使用CMOD / cmod / Cmod(包括chtl文件) + CJMOD / cjmod / CJmod(不包括chtl文件)两个文件夹进行分类  

对于用户来说，他们创建的module文件夹也能够使用分类结构  
值得一提，源代码目录下的模块源码目录Module也可以使用分类结构  

[Import] @Chtl from 具体路径.*  // 导入具体路径下的所有.cmod和.chtl文件  
[Import] @Chtl from 具体路径.*.cmod  // 导入具体路径下的所有.cmod文件  
[Import] @Chtl from 具体路径.*.chtl  // 导入具体路径下的所有.chtl文件  
等价于
[Import] @Chtl from 具体路径/*  // 导入具体路径下的所有.cmod和.chtl文件  
[Import] @Chtl from 具体路径/*.cmod  // 导入具体路径下的所有.cmod文件  
[Import] @Chtl from 具体路径/*.chtl  // 导入具体路径下的所有.chtl文件  

// 导入子模块时，支持使用'/'替代'.'作为路径分隔符  
[Import] @Chtl from Chtholly.*  // 导入Chtholly模块的所有子模块  
[Import] @Chtl from Chtholly.Space  // 导入Chtholly模块中指定的Space子模块  

[Import] @CJmod from 模块名称  // 导入指定名称的CJmod模块  

CJmod与Cmod采用相同的路径搜索策略。  

## 项目结构建议
由于CHTL项目的复杂性，我们强烈建议使用状态机(state) + 策略模式模式(Strategy)来组织CHTL项目  
递归下降也可以，只是会复杂很多  

```chtl
CHTL(项目文件夹，可以换成src)
    -CHTL(CHTL编译器)
        -CHTLContext
        -CHTLGenerator
        -CHTLLexer(内含Lexer，GlobalMap，Token文件)
        -CHTLLoader
        -CHTLManage(如果需要管理器)
        -CHTLNode(内含BaseNode文件，ElementNode，TextNode，CommentNode，TemplateNode，CustomNode，StyleNode，ScriptNode，OriginNode，ImportNode，ConfigNode，NamespaceNode，OperatorNode(delete，insert，use...)等文件)
        -CHTLParser
        -CHTLState
        -CHTLStrategy
	    -CHTLIOStream(如果需要IO流)
        -CMODSystem

    -CHTLJS
        -CHTLJSContext
        -CHTLJSGenerator
        -CHTLJSLexer(Lexer，GlobalMap，Token文件)
        -CHTLJSLoader
        -CHTLJSManage(如果需要管理器)
        -CHTLJSNode(CHTLJSBaseNode等CHTL JS节点)
        -CHTLJSParser
        -CHTLJSState
        -CHTLJSStrategy
	    -CHTLJSIOStream(如果需要IO流)
        -CJMODSystem

    -CSS

    -JS

    -SharedCore(盐桥，共享CHTL和CHTL JS的数据)

    -ThirdParty

    -Util
        FileSystem(文件夹)
	    ZipUtil(文件夹)
	    StringUtil(文件夹)
        
    -Test
        -UtilTest(文件夹，提供UtilTest功能)
        -TokenTest(文件夹，提供TokenPrint和TokenTable功能)
        -AstTest(文件夹，提供ASTPrint，ASTGraph)

    -Module(模块源码文件夹)
```

## 项目流程                     
                    ┌─────────────┐                   ┌─────────────┐
                    │    CHTL     │     盐桥机制       │   CHTL JS   │
                    │  Compiler   │   <---------->    │  Compiler   │
                    │  (手写)     │                    │  (手写)      │
                    └─────────────┘                    └─────────────┘
                        │                                    │
                        └──────────────----------------------┴
                            ▼ CHTL和CHTL JS处理后的完整代码

                            ... 借助外部编译器进行语法验证  
                    ┌─────────────┐ ┌─────────────┐
                    │     CSS     │ │JavaScript   │
                    │  Compiler   │ │  Compiler   │
                    │  (ANTLR)    │ │  (ANTLR)    │
                    └─────────────┘ └─────────────┘
                        │              │
                        └──────────────┴
                              ▼ 
                            ┌───────────
                            │编译结果合并│
                            │(HTML输出) │
                            └──────────

编译器对应关系说明如下：  
- 局部样式（style）→ CHTL编译器  
- 全局样式（style）→ CSS编译器  
- 脚本（script）→ 由CHTL编译器、CHTL JS编译器及JS编译器共同管理  

注意，CHTL项目依托两个外部引入的CSS / JS编译器，你可以使用ANTLR4 / 合适的库引入CSS / JS编译器，不强制使用ANTLR4  
这里有一些可选推荐  ->  ANTLR4，libcss，sassc，V8，QuickJS  

## 语法边界
全局样式块仅允许使用以下语法元素：属性运算，属性条件表达式，模板变量，自定义变量，自定义变量特例化，模板样式组，自定义样式组，无值样式组，自定义样式组特例化，delete属性，delete继承，样式组间继承，生成器注释，全缀名，任意类型原始嵌入（原始嵌入可在任意位置使用），以及通过命名空间引入模板变量，自定义变量，模板样式组，自定义样式组，无值样式组，即from语法  

除局部script外，其他script禁止使用任何CHTL语法，允许例外的元素为：注释及任意类型原始嵌入(二者为特殊存在，可在任意位置使用)  

局部样式块允许使用以下语法元素：模板变量，自定义变量，自定义变量特例化，模板样式组，自定义样式组，无值样式组，自定义样式组特例化，delete属性，delete继承，样式组间继承，生成器注释，全缀名，任意类型原始嵌入(原始嵌入可在任意位置使用)，以及通过命名空间引入模板变量，自定义变量，模板样式组，自定义样式组，无值样式组，即from语句  

局部script允许使用以下CHTL语法元素：模板变量，自定义变量组，变量组特例化，命名空间from，注释及任意类型原始嵌入(注释和原始嵌入为特殊存在，可在任意位置使用)，{{&}}属于CHTL与CHTL JS的交互语法，上述的行为都需要盐桥的介入  

## CJMOD API
### 简介
CJMOD是CHTL项目CHTL JS模块的扩展组件，用于提供模块化分发CHTL JS代码  
CHTL提供了CJMOD API，CJMOD API极其强大，能够高效创建CHTL JS语法  

### CJMOD API
#### 使用案例
```cpp
Arg args = Syntax::analyze("$ ** $");  // 语法分析
args.print();  // 输出-> ["$", "**", "$"]

args.bind("$", [](const std::string& value) {
    return value;
});

args.bind("**", [](const std::string& value) {
    return value;
});

args.bind("$", [](const std::string& value) {
    return value;
});

Arg result = CJMODScanner::scan(args, "**");
result.print();  // 输出-> ["3", "**", "4"]

args.fillValue(result);
std::cout << arg[0].value << std::endl;  // 输出-> 3
std::cout << arg[1].value << std::endl;  // 输出-> **
std::cout << arg[2].value << std::endl;  // 输出-> 4

// args.transform("pow(" + args[0].value + args[2].value + ")");

args.transform("pow(". + 
args.match("$", [](const std::string& value) { return value; }) + 
args.match("$", [](const std::string& value) { return value; }) +
")");

CJMODGenerator::exportResult(args);
```

```chtl
script
{
    console.log(3 ** 4);
}

->

<script>
    console.log(pow(3, 4));
</script>
```

##### Syntax
语法分析类，这个类负责对语法进行解析  

###### analyze
分析语法，返回一个Arg对象，Arg对象包含了解析出的参数列表  

```cpp
Arg args = Syntax::analyze("$ ** $");  // 语法分析
args.print();  // 输出-> ["$", "**", "$"]
```

###### isObject
判断是否是JS对象  

```cpp
Syntax::isObject("{b: 1}");  // 输出-> true
```

###### isFunction
判断是否是JS函数  

```cpp
Syntax::isFunction("function a(){}");  // 输出-> true
```

###### isArray
判断是否是JS数组  

```cpp
Syntax::isArray("[1, 2, 3]");  // 输出-> true
```

###### isCHTLJSFunction
判断是否是CHTL JS函数  

```cpp
Syntax::isCHTLJSFunction("test {test: 1, test2: 2};");  // 输出-> true
```

##### Arg
参数列表类，这个类包含了解析出的参数列表，并且提供了参数的绑定、填充、转换等操作  

###### bind
让一个原子Arg绑定获取值的函数  

```cpp
Arg args = Syntax::analyze("$ ** $");  // 语法分析
args.bind("$", [](const std::string& value) {
    return value;
});

args.bind("**", [](const std::string& value) {
    return Syntax::isCHTLJSFunction(value) ? "" : value;
});
```

###### match
匹配参数列表中的参数  
会自动对匹配到的占位符进行计数 + 1  

例如第一次匹配$时，参数列表中的参数$的计数为1，表示第一个$参数  
第二次匹配$时，参数列表中的参数$的计数+1，表示第二个$参数  

```cpp
args.match("$", [](const std::string& value) { return value; });
```

###### fillValue
填充参数列表的值  

```cpp
Arg result = CJMODScanner::scan(args);
args.fillValue(result);
args.fillValue(Arg(["3", "**", "4"]));
```

###### transform
参数最终输出什么JS代码  

```cpp
args.transform("pow(" + args[0].value + args[2].value + ")");
```

##### CJMODScanner
特殊的扫描器，用于扫描语法片段  

###### scan
扫描语法片段，第二参数为扫描的关键字  

```cpp
Arg result = CJMODScanner::scan(args, "**");
```
scan方法需要拿到真实的代码片段  

#### CJMODGenerator
生成器用于CMJMOD API的接口  

###### exportResult
导出最终的JS代码  

```cpp
CJMODGenerator::exportResult(args);
```

#### AtomArg
原子参数，Arg封装此    

##### $
占位符  

##### $?
可选占位符  

##### $!
必须占位符  

##### $_
无序占位符  

上述占位符可以组合  
例如$!_  

##### ...
不定参数占位符  

##### bind
绑定获取值的函数  

```cpp
args[0].bind([](const std::string& value) {return value;});
```

##### fillValue
填充参数值  

```cpp
args[0].fillValue("3");
args[0].fillValue(3);
```

#### CHTLJSFunction
CHTL JS函数，用于CJMOD API的接口  

##### CreateCHTLJSFunction
封装了原始API构建语法的流程，能够快速构建CHTL JS函数，这些CHTL JS函数天然支持虚对象Vir以及无修饰字符串    
构建的函数天然支持无序键值对，可选键值对，无修饰字面量  

```cpp
CHTLJSFunction func;
CHTLJSFunction::CreateCHTLJSFunction("PrintMylove {url: $!_, mode: $?_}");
```

```chtl
script
{
    PrintMylove({url: "https://www.baidu.com", mode: "auto"});
}

天然支持Vir  
script
{
    Vir test = PrintMylove({url: "https://www.baidu.com", mode: "auto"});
}
```

##### bindVirtualObject
绑定虚对象Vir  
对于不使用CreateCHTLJSFunction创建的，但是符合CHTL JS函数的语法  
可以使用bindVirtualObject手动绑定虚对象Vir，获得虚对象的支持  

```cpp
Syntax::isCHTLJSFunction("PrintMylove {url: $!_, mode: $?_}");  // 输出-> true
CHTLJSFunction::bindVirtualObject("PrintMylove");  // 写函数名称 
```

### 扫描算法
这是我目前已知的两种可用的扫描算法，如果有更好的算法，建议更换更好的扫描算法  

#### 双指针扫描
CJMOD的代码总是以片段出现，双指针扫描一开始两个指针同时位于0位置  
然后预先扫描一个片段是否存在关键字，没有就移动前指针到合适的位置  
然后类似滑动窗口算法同步向前，如果前指针遇到了关键字，就通知后指针准备收集  
确保语法片段能够成功返回CJMOD，使其正确填充参数  

#### 动态感知
例如arg ** arg，识别之后动态识别前后  

## CLI
CHTL提供了两个CLI工具，分别为常规命令行与命令行程序  
常规命令行与我们平时使用的cmd，powershell无异，不具备渲染等特征，主打一个简洁  

命令行程序则支持常规命令行很多不支持的功能，例如画面渲染，RGB，背景图，半透明等  

CLI支持事件循环  

### 默认结构
注意，默认情况下，CHTL编译器不应该提供默认的结构，而是根据用户写了什么内容，生成什么内容  
这是因为CHTL编译器必须高度支持SPA页面，SPA页面通常不具有结构，尽管生成的结构会被Web处理器忽略，但提供纯净的内容更适合  
你可以使用编译器指令--default-struct -> 来输出带有默认结构的HTML代码  

### 内联
默认情况下，CHTL编译后将得到独立的HTML，CSS，JS文件  
你可以使用--inline指令指示编译器生成内联的HTML文件，而非独立的文件  
--inline指令共有三条  

--inline  
--inline-css  
--inline-js  

## VSCode IDE
CHTL项目推荐使用VSCode IDE  
VSCode IDE需要满足下述基本要求  

1. 代码高亮  
2. 代码格式化(JS和CHTL JS使用JS风格代码格式化，其他使用C++风格代码格式化)  
3. 代码提示  
4. 页面预览  
5. 右键浏览器打开  
6. 右键打开文档  
7. 右键导出HTML  
8. 右键导出CSS  
9. 右键导出JS  
10. 实时预览  
11. 内置编译器和官方模块  
12. 自动模块解包和JSON查询表  
13. 右键编译  
14. 解决[]自动补全冲突  
15. 模块语法提示

### 自动模块解包和JSON查询表
如果导入的是CMOD模块，则导出[Export]块的内容，并根据此优化性能，提供语法提示，并创建json表    
如果导入的是CJMOD模块，则根据scan，CHTLJSFunction，analyze这三个函数接收的代码片段，提供语法提示，并创建json表  

## 编译监视器
编译计时器能够监视编译器的编译时间和使用内存，必要时杀死程序，防止对开发者的造成可能的危害  

## CHTL委员会第三期-元素行为与条件渲染
### 行为
```chtl
div
{
    style
    {

    }

    script
    {

    }

    text
    {

    }
}
```
上述是一个常见的CHTL元素  
它的内部具有style，script，text三个块  
这些块被称为行为  
决定了这个元素具有什么样的特征  
例如style块决定了元素具有什么样式  
script块决定了元素具有什么行为  
text块决定了元素具有什么文本内容  

行为是CHTL元素的核心，它控制着CHTL元素的特征，如何显示，如何响应，如何交互  
在默认情况下，行为使用声明块表示，例如style{}，script{}，text{}  

如果你想要引入更多的行为，请尽可能使用声明式语法取代程序式语法  
例如if  

```cpp
if(条件表达式)
{
    // 条件成立时执行的代码
} else if(条件表达式)
{
    // 条件成立时执行的代码
} else
{
    // 条件不成立时执行的代码
}
```

```chtl
if
{
    condition: 与属性相关的条件表达式,
    // 条件成立时执行的代码，写属性
    width: 100px,
    display: block,
}
else if
{
    condition: 与属性相关的条件表达式,
    // CSS代码
}
else
{
    // CSS代码
}
```

当然，这只是一种建议，你也完全可以使用原本的程序式语法  
或者两者同时提供  

### 条件渲染
现在，你可以在元素内部定义if块，用于条件渲染  
条件渲染属于静态特征  

```chtl
html
{
    head
    {

    }

    body
    {
        if
        {
            condition: html.width < 500px,
            display: none,
        }
    }
}
```

### 动态条件渲染
静态条件渲染必然是存在限制的，为此，CHTL JS提供了动态条件渲染  

```chtl
html
{
    head
    {

    }

    body
    {
        if
        {
            condition: {{html}}.width < 500px,  // 使用动态属性 / 响应式值时，使用动态条件渲染
            display: none,
        }
    }
}
```
