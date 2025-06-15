lexer grammar Lexer;  //lexer关键字意味着这是一个词法规则文件，名称是Lexer，要与文件名相同

//关键字
Int: 'int';

//字面量
IntLiteral: [0-9]+;

//操作符
Semicolon: ';';
Assignment: '=';
Or: '||';
And: '&&';
Equal: '==';
NotEqual: '!=';
GT: '>';
GE: '>=';
LT: '<';
LE: '<=';
Plus: '+';
Minus: '-';
Star: '*';
Slash: '/';
LParen: '(';
RParen: ')';

//标识符
Identifier: [a-zA-Z_]([a-zA-Z_]|[0-9])*;

//空白字符，抛弃
Whitespace:         [ \t]+ -> skip;
Newline:            ( '\r' '\n'?|'\n')-> skip;


