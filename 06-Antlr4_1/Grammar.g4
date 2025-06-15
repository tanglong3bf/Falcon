grammar Grammar;
import Lexer;

prog : statement+;
statement : (intDeclare | assign | orExp ) ';';
intDeclare : 'int' Identifier ( '=' assign )?;
assign : (Identifier '=' assign) | orExp;
orExp : andExp ( '||' andExp )*;
andExp : equalExp ( '&&' equalExp )*;
equalExp : relExp ( ('==' | '!=') relExp )*;
relExp : addExp ( ('>' | '<' | '>=' | '<=') addExp )*;
addExp : mulExp ( ('+' | '-') mulExp )*;
mulExp : priExp ( ('*' | '/') priExp )*;
priExp : Identifier | IntLiteral | '(' assign ')';
