lexer grammar FalconLexer;

// 类型
INT: 'int';

// 关键字
IF: 'if';
ELSE: 'else';
DO: 'do';
WHILE: 'while';
FOR: 'for';
BREAK: 'break';
CONTINUE: 'continue';
SWITCH:'switch';
CASE: 'case';
DEFAULT: 'default';

// 标识符
IDENTIFIER: Letter LetterOrDigit*; 

// 字面量
DECIMAL_LITERAL: ('0' | [1-9] (Digits? | '_'+ Digits)) [lL]?; 
HEX_LITERAL: '0' [xX] [0-9a-fA-F] ([0-9a-fA-F_]* [0-9a-fA-F])? [lL]?; 
OCTAL_LITERAL: '0' '_'* [0-7] ([0-7_]* [0-7])? [lL]?; 
BINARY_LITERAL: '0' [bB] [01] ([01_]* [01])? [lL]?; 

// 运算符
// 双目运算符
PLUS: '+';
MINUS: '-';
MULTIPLY: '*';
DIVIDE: '/';
MODULUS: '%';
L_SHIFT: '<<';
R_SHIFT: '>>';
EQUAL: '==';
NOT_EQUAL: '!=';
LESS: '<';
LESS_EQUAL: '<=';
GREATER: '>';
GREATER_EQUAL: '>=';
BIT_AND: '&';
BIT_OR: '|';
BIT_XOR: '^';
AND: '&&';
OR: '||';
// 赋值运算符
ASSIGN: '=';
PLUS_ASSIGN: '+=';
MINUS_ASSIGN: '-=';
MULTIPLY_ASSIGN: '*=';
DIVIDE_ASSIGN: '/=';
MODULUS_ASSIGN: '%=';
L_SHIFT_ASSIGN: '<<=';
R_SHIFT_ASSIGN: '>>=';
BIT_AND_ASSIGN: '&=';
BIT_OR_ASSIGN: '|=';
BIT_XOR_ASSIGN: '^=';
// 单目运算符
INCREMENT: '++';
DECREMENT: '--';
NOT: '!';
NEGATE: '~';
// 三目运算符
TERNARY: '?';
COLON: ':';
// 界符
L_PAREN: '(';
R_PAREN: ')';
L_BRACE: '{';
R_BRACE: '}';
L_BRACKET: '[';
R_BRACKET: ']';
COMMA: ',';
SEMI: ';';
DOT: '.';

// 注释
LINE_COMMENT: '//' ~[\r\n]* -> skip;
BLOCK_COMMENT: '/*' .*? '*/' -> skip;

// 空白字符
WS: [ \t\r\n]+ -> skip;

// 代码片段
fragment ExponentPart
    : [eE] [+-]? Digits
    ;

fragment EscapeSequence
    : '\\' [btnfr"'\\]
    | '\\' ([0-3]? [0-7])? [0-7]
    | '\\' 'u'+ HexDigit HexDigit HexDigit HexDigit
    ;

fragment HexDigits
    : HexDigit ((HexDigit | '_')* HexDigit)?
    ;

fragment HexDigit
    : [0-9a-fA-F]
    ;

fragment Digits
    : [0-9] ([0-9_]* [0-9])?
    ;

fragment LetterOrDigit
    : Letter
    | [0-9]
    ;

fragment Letter
    : [a-zA-Z$_]
    | ~[\u0000-\u007F\uD800-\uDBFF]
    | [\uD800-\uDBFF] [\uDC00-\uDFFF]
    ;
