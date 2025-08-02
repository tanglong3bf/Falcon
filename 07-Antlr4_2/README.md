# 用 Antlr4 重构我们的脚本语言

## 安装运行库

首先我们需要准备好antlr4的运行时库。

```
sudo apt-get install antlr4-runtime-cpp-dev
```

在我本地的环境（Ubuntu20.04），它的头文件被安装到了`/usr/local/include/antlr4-runtime/`中。

## 定义脚本语言的语法

我这里并不会一次性完成所有语法的定义，比如不包含函数定义、类定义、闭包。

### 注释

包括单行注释和多行注释。

```
// 单行注释
/* 多行注释 */
```

### 类型定义

基本数据类型只支持 int。（本来打算支持好多好多类型的，但是好麻烦，就暂时放弃了）

### 运算符

基础的双目运算符：+, -, \*, /, %, <<, >>, ==, !=, >, <, >=, <=, &, |, ^, &&, ||, =, +=, -=, \*=, /=, %=, <<=, >>=, &=, |=, ^=

单目运算符：++, --, +, -, ~, !

三目运算符：? :

一些边界符：,, ., ', ", ( ), { }, \[ \], ;

### 语句

1. if-else语句：if (expr) { stmt } else { stmt }
2. for语句：for (init; expr; update) { stmt }
3. while语句：while (expr) { stmt }
4. do-while语句：do { stmt } while (expr);
5. switch语句（未完成）：switch (expr) { case value1: stmt; break; case value2: stmt; break; default: stmt; }
6. break语句：break;
7. continue语句：continue;
8. 表达式语句：expr;，会输出表达式的值

## 编写词法/语法定义文件

我们需要编写词法/语法定义文件，来定义我们的脚本语言的语法。

1. 词法文件名：FalconLexer.g4
2. 语法文件名：FalconScript.g4

**注意：词法文件名不能是语法文件名+Lexer.g4，否则会导致冲突。**

## 使用 Antlr4 生成代码

### 生成词法分析器

```bash
antlr4 -Dlanguage=Cpp FalconLexer.g4
```

它会生成四个文件：

```bash
$ tree .
.
├── FalconLexer.cpp
├── FalconLexer.h
├── FalconLexer.interp
└── FalconLexer.tokens
```

其中 FalconLexer.cpp 和 FalconLexer.h 包含了词法分析器的实现，剩余两个文件俺也看不懂，应该是会读取里面的内容。

生成完了看一眼内容就行，后面用不到它们，所以可以直接删除。建议删除，不然看起来更乱套。

后面要用的词法分析器的实现文件以及*.interp和*.tokens文件，文件名和它不一样。

### 同时生成词法和语法分析器

```bash
antlr4 -Dlanguage=Cpp FalconScript.g4
```

它会生成一堆文件：

```bash
# 顺序有所调整
$ tree .
.
├── FalconScriptLexer.interp
├── FalconScriptLexer.tokens
├── FalconScriptLexer.cpp
├── FalconScriptLexer.h
├── FalconScript.interp
├── FalconScript.tokens
├── FalconScriptBaseListener.cpp
├── FalconScriptBaseListener.h
├── FalconScriptListener.cpp
├── FalconScriptListener.h
├── FalconScriptParser.cpp
└── FalconScriptParser.h
```

其中FalconScriptLexer.\*和上一步骤生成的四个文件内容一致，只是文件名发生了变化。

FalconScript.interp和FalconScript.tokens文件也和FalconScriptLexer.\*一样。

实际上只是多了六个文件：\*Listener.(cpp|h)，\*BaseListener.(cpp|h)，\*Parser.(cpp|h)。

1. FalconScriptListener.(cpp|h) 定义了语法分析器的监听器接口。
2. FalconScriptBaseListener.(cpp|h) 定义了语法分析器的监听器基类。
3. FalconScriptParser.(cpp|h) 定义了语法分析器。

一些猜测：我们可以尝试生成Java的代码，可以发现\*Listener.java是一个接口，\*BaseListener.java是一个类，其内部的函数都是空实现。

C++定义的这两个类都是抽象类，里面都是只有纯虚函数，没有具体实现。

基本使用方式：FalconScriptParser类用于完成语法分析，其内部会用到FalconScriptLexer类进行词法分析。我们可以通过继承FalconScriptBaseListener类，实现自己的监听器。

**但是！**这种方式不够优雅。

### 使用 visitor 模式

```bash
antlr4 -Dlanguage=Cpp -visitor FalconScript.g4
```

生成了一大堆零四个文件

```bash
$ tree .
.
├── ... # 前面的都一样
├── FalconScriptBaseVisitor.cpp
├── FalconScriptBaseVisitor.h
├── FalconScriptVisitor.cpp
└── FalconScriptVisitor.h
```

1. FalconScriptVisitor.(cpp|h)定义了 visitor 的接口。
2. FalconScriptBaseVisitor.(cpp|h)定义了 visitor 的基类。

我们可以通过继承 FalconScriptBaseVisitor 类，实现自己的 visitor，完成对语法树的遍历。

### Listener 和 Visitor 的区别

两者都可以用于完成对语法树的遍历，Listener更简单，Visitor则是更灵活。

Listener 会按照深度优先遍历语法树，只会访问每一个子节点两次，一次进入，一次退出。

Visitor 由我们自己编写的代码决定访问哪些节点以及访问顺序，会访问每一个子节点零到多次，比如 if-else 语句只会访问其中一个子分支，比如 for 循环 会多次访问循环体。

同时 Visitor 允许访问完子节点后获取返回值，而 Listener 则不允许。

一般而言，我们可以用 Listener 完成一些语义检查，Visitor 用于解释执行。

## 目录结构

从这一节开始，代码量膨胀了好多，而且有大部分内容都是由\*.g4文件生成的，这种中间文件不建议交给git来管理，而是应该在编译的过程中生成。

但我又不太会使用 CMake ，只能使用老古董 Makefile 了。

同时中间文件有亿点多，打算将其放到临时目录中，方便编译。

具体看 ./src/Makefile 吧。

## 吐亿点槽

我在实现的过程中，本来打算实现好多好多语法，但是发现有点力不从心。

~~我定义了一个类，FalconVariable 继承自 antlrcpp::Any，然后实现了一大坨的运算符重载，这样expression节点的解析会很方便。~~

**从原本的自定义类型修改为直接使用 antlrcpp::Any 类型。**

我一开始打算实现好多好多数据类型，甚至专门实现了一个枚举，具体可以看：

```cpp
enum class FalconType
{
    Integer,  // int32_t
};
```

但是后来发现，运算符重载的时候要判断类型，分支有亿点多，遂放弃，最终只保留了 int。顺带着把很多语法也都砍掉了（暂时的）。

当然，有点后知后觉的发现，这其实属于语义分析。。。

~~同时 `MyVisitor` 的 `variables_` 也需要修改，暂时放弃了。~~

已经修改为直接存储 `antlrcpp::Any` 类型。

考虑到类 `FalconScriptBaseVisitor` 的定义，会随着我修改\*.g4文件而发生变化，前面也说过要把这些中间文件放到.gitignore里，所以需要继承 `FalconScriptBaseVisitor`，在子类中重写我们自己的实现。

实现过程中，我最早实现的是`visitExpression()`~~，这里就会用到FalconAny类的运算符重载~~。

逐步往后实现的过程中，越来越得心应手，但是到了for循环的时候卡壳了，

enhancedForControl 节点实质上是在遍历数组，之前已经删掉了数组，那么这个语法也只能砍掉了。

然后自然就是forControl，实现它的过程中又小卡了一下，我拿不到循环体啊。

百思不得其解，最后参考了老师的实现，是在visitStatement()函数中实现的。

而我此时还没有实现它，我把它放到了最后。

最后实现visitStatement()，又删了switch-case

~~预计此目录会再更新几次，把数组、范围for循环都实现一下。~~

放弃了。。。

switch-case……看情况吧。

## 当前的效果

具体可以看 ./src/scripts/\*.falc
