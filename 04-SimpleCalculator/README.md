# 优先级和结合性

本节课程介绍了除加法和乘法外的更多的运算符，包括赋值运算、逻辑运算（与、或）、比
较运算（相等、不等、大于、小于、大于等于、小于等于）、算术运算（加、减、乘、除）

优先级如下：

| 级别 | 运算符       | 结合性   |
| ---- | ------------ | -------- |
| 1    | \*、/        | 从左到右 |
| 2    | +、-         | 从左到右 |
| 3    | >、>=、<、<= | 从左到右 |
| 4    | ==、!=       | 从左到右 |
| 5    | &&           | 从左到右 |
| 6    | \|\|         | 从左到右 |
| 7    | =            | 从右到左 |

## 语法规则

对应的上下文无关文法：

```
exp -> or | or = exp
or -> and | or || and
and -> equal | and && equal
equal -> rel | equal == rel | equal != rel
rel -> add | rel > add | rel < add | rel >= add | rel <= add
add -> mul | add + mul | add - mul
mul -> pri | mul * pri | mul / pri
// 使用括号来改变优先级
pri -> Id | Literal | (exp)
```

对上述语法规则，补充上一节的整数定义，并改用 EBNF 来进行表示：

```
// 补上了分号，没有分号感觉怪怪的
prog ::= (intDeclare | assign) ";"
intDeclare ::= "int" Id ( "=" assign )?
// exp 改为了 assign，它明显就是赋值操作
assign ::= or ( "=" assign )?
or ::= and ( "||" and )*
and ::= equal ( "&&" equal )*
equal ::= rel ( ("==" | "!=") rel )*
rel ::= add ( (">" | "<" | ">=" | "<=") add )*
add ::= mul ( ("+" | "-") mul )*
mul ::= pri ( ("*" | "/") pri )*
// 使用括号来改变优先级
pri ::= Id | Literal | "(" assign ")"
```

> 注意：`assign ::= or ( "=" assign )?`这一条，可能会导致出现`a || b = c || d`这种语句
> 被识别为合法的表达式，但实际上左侧明显不能被赋值。
> 尝试过对其进行修改，但是发现修改后的版本需要回溯，故暂不修改。

### 另一种表示方法

顺带一提，网络上的资料里对于EBNF的表示方法的定义，大多和专栏里的不一样，如果按照
网络上的资料重新编写上述语法规则，可能会是这样：

```
intDeclare ::= "int" Id [ "=" assign ] ";"
assign ::= or [ "=" assign ]
or ::= and { "||" and }
// ...
```

即用方括号(`[]`)表示可选，用花括号(`{}`)表示0-n次重复。

如果是重复1-n次，前者可以`(...)+`，后者只能用`...{...}`。

两种方式都是EBNF定义的标准方式，但具体选择哪种，取决于个人喜好。

## 词法规则

- Identifier: `[a-zA-Z_]([a-zA-Z_]|[0-9])*`
- Int: `'int'`
- IntLiteral: `[0-9]+`
- Semicolon: `';'` // 新增
- Assignment: `'='`
- Or: `'||'` // 新增
- And: `'&&'` // 新增
- Equal: `'=='` // 新增
- NotEqual: `'!='` // 新增
- GT: `'>'`
- GE: `'>='`
- LT: `'<'` // 新增
- LE: `'<='` // 新增
- Plus: `'+'`
- Minus: `'-'`
- Star: `'*'`
- Slash: `'/'`
- LParen: `'('` // 新增
- RParen: `')'` // 新增
