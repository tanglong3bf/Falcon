# 一个简单的计算器

## 词法规则

词法规则与 [02-SimpleLexer](../02-SimpleLexer/README.md) 相同。

## 语法规则

```
intDeclare
    : Int Identifier (Assignment additive)?
    ;

additive
    : multiplicative
    | multiplicative Plus additive
    ;

multiplicative
    : IntLiteral
    | IntLiteral Star multiplicative
    ;
```

此语法规则，可以解决左递归问题，但是却使用了错误的右结合。
