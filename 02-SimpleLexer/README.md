# 02-手动完成的简单的词法分析器

支持的词法规则：

- Identifier: `[a-zA-Z_]([a-zA-Z_]|[0-9])*`
- Int: `'int'`
- IntLiteral: `[0-9]+`
- Assignment: `'='`
- GT: `'>'`
- GE: `'>='`
- Plus: `'+'`
- Minus: `'-'`
- Star: `'*'`
- Slash: `'/'`

词法分析器使用状态机，额外需要注意的是`int`的三个中间状态。

## 其他可选择方案

我们可以准备一个哈希表，记录下所有的关键字：

```cpp
std::unrodered_set<std::string> keywords = {"int",
                                            "if",
                                            "else",
                                            "while",
                                            "return"};
```

之后只需要按照正常的标识符解析就行，解析完标识符后，检查其是否在哈希表中。如果
在，就是关键字，不在，就是标识符。

可以减少很多中间状态。
