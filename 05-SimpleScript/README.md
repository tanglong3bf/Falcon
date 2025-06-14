# 简单的REPL

## 期望的效果

语法规则还得进一步调整，先来展示一下期望的效果吧：

1. 定义变量：

```
// 变量类型只有int，默认值为0
// 其中(*)表示是一个新的变量
> int age;
(*)age: 0

// 可以赋初始值
> int height = 180;
(*)height: 180
```

2. 为变量赋值：

```
// 前面定义过age
> age = 18;
age: 18

> a = 10;
Error: variable 'a' is not defined.

> int a; int b;
(*)a: 0
(*)b: 0

// 可以链式赋值
> a = b = 10;
b: 10 // 赋值号从右向左执行，所以是b, a的顺序
a: 10
```

3. 输出变量：

```
> age;
age: 18
```

4. 执行一个表达式：

```
> 2 + 3 * 4;
14

> (2 + 3) * 4;
20

> 12+233-45*6/2;
110

// age=18
// 没做bool值，用1/0表示true/false
> age>=18;
1

// 当然也可以把结果存储到变量中
> int isAdult = age >= 18;
(*)isAdult: 1

> int isMe = isAdult && height >= 180;
(*)isMe: 1
```

5. 退出REPL：

```
// 字符串硬解析
> exit();
Bye!
```

我考虑过在同一条语句中定义多个变量，比如`int a=1, b=2, c;`，尝试后没搓出来。
但是可以在同一行写多条语句：`int a=1; int b=2; int c;`。

## 语法调整

先把之前的语法规则贴出来：

```
prog ::= (intDeclare | assign) ";"
intDeclare ::= "int" Id ( "=" assign )?
assign ::= or ( "=" assign )?
or ::= and ( "||" and )*
and ::= equal ( "&&" equal )*
equal ::= rel ( ("==" | "!=") rel )*
rel ::= add ( (">" | "<" | ">=" | "<=") add )*
add ::= mul ( ("+" | "-") mul )*
mul ::= pri ( ("*" | "/") pri )*
pri ::= Id | Literal | "(" assign ")"
```

1. 同一个程序可以有多条语句

```
prog ::= statement+
```

2. 每一条语句可能是变量的定义、赋值、表达式，最后统一补上一个分号作为语句的结尾：

```
statement ::= (intDeclare | assign | or ) ";"
```

3. 变量的定义语法，可选是否赋初始值：

```
intDeclare ::= "int" Id ( "=" assign )?
```

4. 赋值运算，这里有所修改：

```
assign ::= Id "=" (or | assign)
```

`or`可以以`Id`开头，`assign`必然以`Id`开头，所以需要回溯，具体的看代码吧。

**最终结果：**

```
prog ::= statement+
statement ::= (intDeclare | assign | or ) ";"
intDeclare ::= "int" Id ( "=" assign )?
assign ::= (Id "=" assign) | or
or ::= and ( "||" and )*
and ::= equal ( "&&" equal )*
equal ::= rel ( ("==" | "!=") rel )*
rel ::= add ( (">" | "<" | ">=" | "<=") add )*
add ::= mul ( ("+" | "-") mul )*
mul ::= pri ( ("*" | "/") pri )*
pri ::= Id | Literal | "(" assign ")"
```

## REPL 的实现

```cpp
std::unordered_map<std::string, int> variables_;
```
