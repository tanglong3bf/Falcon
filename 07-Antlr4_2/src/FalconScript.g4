grammar FalconScript;

import FalconLexer;

prog
    : blockStatement*
	;

block
    : '{' blockStatement* '}'
	;

blockStatement
    : statement
	| variableDeclarators ';'
	;

statement
    : blockLabel=block
    | IF parExpression statement (ELSE statement)?
    | FOR '(' forControl ')' statement
    | WHILE parExpression statement
    | DO statement WHILE parExpression ';'
    | BREAK ';'
    | CONTINUE ';'
    | SEMI
    | statementExpression=expression ';'
    ;

expression
    : primary
    | expression postfix=('++' | '--')
    | prefix=('+'|'-'|'++'|'--') expression
    | prefix=('~'|'!') expression
    | expression bop=('*'|'/'|'%') expression  
    | expression bop=('+'|'-') expression 
    | expression bop=('<<' | '>>') expression
    | expression bop=('<=' | '>=' | '>' | '<') expression
    | expression bop=('==' | '!=') expression
    | expression bop='&' expression
    | expression bop='^' expression
    | expression bop='|' expression
    | expression bop='&&' expression
    | expression bop='||' expression
    | expression bop='?' expression ':' expression
    | <assoc=right> expression
      bop=('=' | '+=' | '-=' | '*=' | '/=' | '&=' | '|=' | '^=' | '>>=' | '<<=' | '%=')
      expression
    ;

primary
    : '(' expression ')'
    | literal
    | IDENTIFIER
    ;

literal
    : integerLiteral
    ;

integerLiteral
    : DECIMAL_LITERAL
    | HEX_LITERAL
    | OCTAL_LITERAL
    | BINARY_LITERAL
    ;

typeType
    : primitiveType
    ;

primitiveType
    : INT
    ;

variableDeclarators
    : typeType variableDeclarator (',' variableDeclarator)*
    ;

variableDeclarator
    : variableDeclaratorId ('=' variableInitializer)?
    ;

variableDeclaratorId
    : IDENTIFIER
    ;

variableInitializer
    : expression
    ;


parExpression
    : '(' expression ')'
    ;

forControl
    : forInit? ';' expression? ';' forUpdate=expressionList?
    ;

forInit
    : variableDeclarators
    | expressionList
    ;

expressionList
    : expression (',' expression)*
    ;
