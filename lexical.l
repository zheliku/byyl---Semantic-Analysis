%{
    #include <stdlib.h>
    #include <stdbool.h>
    #include <string.h>
    #include "tree.h"
    #define YYSTYPE Node*
    #include "syntax.tab.h"
    int lexical_correct = 1;
%}

%option yylineno

digit [0-9]
letter [a-zA-Z]
unsignedint [1-9]{digit}*

/* int 类型匹配 */
Dec_number [+-]?(0|{unsignedint})
Oct_number [+-]?0(0|([1-7][0-7]*))
Hex_number [+-]?(0(x|X))(0|([1-9A-Fa-f][0-9A-Fa-f]*))
Illegal_Oct [+-]?0[0-7]*[89]+[0-9]*
Illegal_Hex [+-]?0[Xx][0-9a-fA-F]*[g-zG-Z]+[0-9a-zA-Z]*

/* float 类型匹配 */
Float1 [+-]?({digit}+)?\.{digit}+?
Float2 [+-]?({digit}+)[eE][+-]?({digit})+
Float3 [+-]?({digit}+)?\.({digit}+)?([eE][+-]?{digit}+)
Float {Float1}|{Float2}|{Float3}

/* 其余终结符的匹配 */
ID ({letter}|_)({letter}|_|{digit})*
Semi ;
Comma ,
Assign_op =
Rel_op (>)|(<)|(>=)|(<=)|(==)|(!=)
Plus \+
Minus -
Star \*
Div \/
And &&
Or \|\|
Dot \.
Not !
Lp \(
Rp \)
Lb \[
Rb \]
Lc \{
Rc \}
TYPE (int|float)
STRUCT struct
IF if
ELSE else
RETURN return
WHILE while
Blank [ \r\t]+

%%
 /*-----------------------------------------|
 |                跳过单行注释                |
 |-----------------------------------------*/
"//" {
    char c;
    while ((c = input()) != '\n');
}

 /*-----------------------------------------|
 |                跳过多行注释                |
 |-----------------------------------------*/
"/*" {
    char c;
    while ((c = input()) != EOF) {
        if (c == '*') {
            c = input();
            if (c == '/'){
                break;
            }
        }
    }
    if (c == EOF) printf("Error type B at Line %d: LACK OF */.\n", yylineno);
}

{Dot} {
    yylval = createNode(_DOT, yylineno);
    yylval->idName = strdup(yytext);
    return DOT;
}

{Dec_number} {
    yylval = createNode(_INT, yylineno);
    yylval->intValue = strtol(yytext, NULL, 10);
    return INT;
}

{Oct_number} {
    yylval = createNode(_INT, yylineno);
    yylval->intValue = strtol(yytext, NULL, 8);
    return INT;
}

{Hex_number} {
    yylval = createNode(_INT, yylineno);
    yylval->intValue = strtol(yytext, NULL, 16);
    return INT;
}

{Illegal_Oct} {
    printf("Error type A at Line %d: Illegal oct number \'%s\' \n", yylineno, yytext);
    lexical_correct = 0;
    yylval = createNode(_INT, yylineno);
    yylval->intValue = 0;
    return INT;
}

{Illegal_Hex} {
    printf("Error type A at Line %d: Illegal hex number \'%s\' \n", yylineno, yytext);
    lexical_correct = 0;
    yylval = createNode(_INT, yylineno);
    yylval->intValue = 0;
    return INT;
}

{Float} {
    yylval = createNode(_FLOAT, yylineno);
    yylval->floatValue = strtof(yytext, NULL);
    return FLOAT;
}

{TYPE} {
    yylval = createNode(_TYPE, yylineno);
    yylval->idName = strdup(yytext);
    return TYPE;
}

{STRUCT} {
    yylval = createNode(_STRUCT, yylineno);
    yylval->idName = strdup(yytext);
    return STRUCT;
}

{RETURN} {
    yylval = createNode(_RETURN, yylineno);
    yylval->idName = strdup(yytext);
    return RETURN;
}

{IF} {
    yylval = createNode(_IF, yylineno);
    yylval->idName = strdup(yytext);
    return IF;
}

{ELSE} {
    yylval = createNode(_ELSE, yylineno);
    yylval->idName = strdup(yytext);
    return ELSE;
}

{WHILE} {
    yylval = createNode(_WHILE, yylineno);
    yylval->idName = strdup(yytext);
    return WHILE;
}

{ID} {
    yylval = createNode(_ID, yylineno);
    yylval->idName = strdup(yytext);
    return ID;
}

{Semi} {
    yylval = createNode(_SEMI, yylineno);
    yylval->idName = strdup(yytext);
    return SEMI;
}

{Comma} {
    yylval = createNode(_COMMA, yylineno);
    yylval->idName = strdup(yytext);
    return COMMA;
}

{Assign_op} {
    yylval = createNode(_ASSIGNOP, yylineno);
    yylval->idName = strdup(yytext);
    return ASSIGNOP;
}

{Rel_op} {
    yylval = createNode(_RELOP, yylineno);
    yylval->idName = strdup(yytext);
    return RELOP;
}

{Plus} {
    yylval = createNode(_PLUS, yylineno);
    yylval->idName = strdup(yytext);
    return PLUS;
}

{Minus} {
    yylval = createNode(_MINUS, yylineno);
    yylval->idName = strdup(yytext);
    return MINUS;
}

{Star} {
    yylval = createNode(_STAR, yylineno);
    yylval->idName = strdup(yytext);
    return STAR;
}

{Div} {
    yylval = createNode(_DIV, yylineno);
    yylval->idName = strdup(yytext);
    return DIV;
}

{And} {
    yylval = createNode(_AND, yylineno);
    yylval->idName = strdup(yytext);
    return AND;
}

{Or} {
    yylval = createNode(_OR, yylineno);
    yylval->idName = strdup(yytext);
    return OR;
}

{Not} {
    yylval = createNode(_NOT, yylineno);
    yylval->idName = strdup(yytext);
    return NOT;
}

{Lp} {
    yylval = createNode(_LP, yylineno);
    yylval->idName = strdup(yytext);
    return LP;
}

{Rp} {
    yylval = createNode(_RP, yylineno);
    yylval->idName = strdup(yytext);
    return RP;
}

{Lb} {
    yylval = createNode(_LB, yylineno);
    yylval->idName = strdup(yytext);
    return LB;
}

{Rb} {
    yylval = createNode(_RB, yylineno);
    yylval->idName = strdup(yytext);
    return RB;
}

{Lc} {
    yylval = createNode(_LC, yylineno);
    yylval->idName = strdup(yytext);
    return LC;
}

{Rc} {
    yylval = createNode(_RC, yylineno);
    yylval->idName = strdup(yytext);
    return RC;
}

{Blank} {} // 忽略空格和 tab

. {printf("Error type A at Line %d: mysterious characters \'%s\' \n", yylineno, yytext);} // 对于未定义字符报错处理

\n {}

%%
