%{
    #include "syntax.tab.h"
    #include "tree.h"

    extern int errors;
    YYLTYPE yylloc;

    // 初始化列数
	int yycolumn = 1;

    // 默认制表符的长度
    #define TABLEN 4

    // 每次定义用户操作之前执行的代码
    #define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \
        yylloc.first_column = yycolumn; \
        yylloc.last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng;
%}

%option yylineno

digit [0-9]
letter [a-zA-Z]
unsignedint [1-9]{digit}*

/* int 类型匹配 */
INT10 [+-]?(0|{unsignedint})
INT8 [+-]?0(0|([1-7][0-7]*))
INT16 [+-]?(0(x|X))(0|([1-9A-Fa-f][0-9A-Fa-f]*))
INT {INT10}|{INT8}

/* float 类型匹配 */
FLOAT1 [+-]?({digit}+)?\.{digit}+?
FLOAT2 [+-]?({digit}+)[eE][+-]?({digit})+
FLOAT3 [+-]?({digit}+)?\.({digit}+)?([eE][+-]?{digit}+)
FLOAT {FLOAT1}|{FLOAT2}|{FLOAT3}

/* 其余终结符的匹配 */
ID ({letter}|_)({letter}|_|{digit})*
SEMI ;
COMMA ,
ASSIGNOP =
RELOP (>|<|>=|<=|==|!=)
PLUS \+
MINUS \-
STAR \*
DIV \/
AND &&
OR \|\|
DOT \.
NOT !
TYPE (int|float)
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
STRUCT struct
IF if
ELSE else
RETURN return
WHILE while
LF \n
OTHER [\r]
TAB [\t]
SPACE [ ]

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


 /*-----------------------------------------|
 |          终结符的匹配及对应的其操作          |
 |-----------------------------------------*/
{DOT} { 
    struct Node *pNode = createNode(_DOT, yylineno);
    yylval = pNode;
    pNode->id_name = strdup(yytext);
    return DOT; 
}
{TYPE} { 
    struct Node *pNode = createNode(_TYPE, yylineno);
    yylval = pNode;
    pNode->id_name = strdup(yytext);
    return TYPE; 
}
{INT} { 
    struct Node *pNode = createNode(_INT, yylineno);
    yylval = pNode;
    pNode->intValue = atoi(yytext);
    return INT; 
}
{FLOAT} { 
    struct Node *pNode = createNode(_FLOAT, yylineno);
    yylval = pNode;
    pNode->floatValue = atof(yytext);
    return FLOAT; 
}
{SEMI} { 
    struct Node *pNode = createNode(_SEMI, yylineno);
    yylval = pNode;
    pNode->id_name = strdup(yytext);
    return SEMI; 
}
{COMMA} { 
    struct Node *pNode = createNode(_COMMA, yylineno);
    yylval = pNode;
    pNode->id_name = strdup(yytext);
    return COMMA; 
}
{ASSIGNOP} { 
    struct Node *pNode = createNode(_ASSIGNOP, yylineno);
    yylval = pNode;
    pNode->id_name = strdup(yytext);
    return ASSIGNOP; 
}
{RELOP}  { 
    struct Node *pNode = createNode(_RELOP, yylineno);
    yylval = pNode;
    pNode->id_name = strdup(yytext);
    return RELOP; 
}
{PLUS} { 
    struct Node *pNode = createNode(_PLUS, yylineno);
    yylval = pNode;
    pNode->id_name = strdup(yytext);
    return PLUS; 
}
{MINUS} { 
    struct Node *pNode = createNode(_MINUS, yylineno);
    yylval = pNode;
    pNode->id_name = strdup(yytext);
    return MINUS; 
}
{STAR} { 
    struct Node *pNode = createNode(_STAR, yylineno);
    yylval = pNode;
    pNode->id_name = strdup(yytext);
    return STAR; 
}
{DIV} { 
    struct Node *pNode = createNode(_DIV, yylineno);
    yylval = pNode;
    pNode->id_name = strdup(yytext);
    return DIV; 
}
{AND} { 
    struct Node *pNode = createNode(_AND, yylineno);
    yylval = pNode;
    pNode->id_name = strdup(yytext);
    return AND; 
}
{OR} { 
    struct Node *pNode = createNode(_OR, yylineno);
    yylval = pNode;
    pNode->id_name = strdup(yytext);
    return OR; 
}
{NOT} { 
    struct Node *pNode = createNode(_NOT, yylineno);
    yylval = pNode;
    pNode->id_name = strdup(yytext);
    return NOT; 
}
{LP} { 
    struct Node *pNode = createNode(_LP, yylineno);
    yylval = pNode;
    pNode->id_name = strdup(yytext);
    return LP; 
}
{RP} { 
    struct Node *pNode = createNode(_RP, yylineno);
    yylval = pNode;
    pNode->id_name = strdup(yytext);
    return RP; 
}
{LB} { 
    struct Node *pNode = createNode(_LB, yylineno);
    yylval = pNode;
    pNode->id_name = strdup(yytext);
    return LB; 
}
{RB} { 
    struct Node *pNode = createNode(_RB, yylineno);
    yylval = pNode;
    pNode->id_name = strdup(yytext);
    return RB; 
}
{LC} { 
    struct Node *pNode = createNode(_LC, yylineno);
    yylval = pNode;
    pNode->id_name = strdup(yytext);
    return LC; 
}
{RC}  { 
    struct Node *pNode = createNode(_RC, yylineno);
    yylval = pNode;
    pNode->id_name = strdup(yytext);
    return RC; 
}
{STRUCT} { 
    struct Node *pNode = createNode(_STRUCT, yylineno);
    yylval = pNode;
    pNode->id_name = strdup(yytext);
    return STRUCT; 
}
{RETURN} { 
    struct Node *pNode = createNode(_RETURN, yylineno);
    yylval = pNode;
    pNode->id_name = strdup(yytext);
    return RETURN; 
}
{IF} { 
    struct Node *pNode = createNode(_IF, yylineno);
    yylval = pNode;
    pNode->id_name = strdup(yytext);
    return IF; 
}
{ELSE} { 
    struct Node *pNode = createNode(_ELSE, yylineno);
    yylval = pNode;
    pNode->id_name = strdup(yytext);
    return ELSE; 
}
{WHILE} { 
    struct Node *pNode = createNode(_WHILE, yylineno);
    yylval = pNode;
    pNode->id_name = strdup(yytext);
    return WHILE; 
}

{ID} { 
    struct Node *pNode = createNode(_ID, yylineno);
    yylval = pNode;
    pNode->id_name = strdup(yytext);
    return ID; 
}
{LF} { yycolumn = 1; }           /* 换行后，更新列数 */
{OTHER} { }                      /* 其他字符匹配不给予操作 */
{TAB} { yycolumn += TABLEN; }    /* 默认制表符长度为 4 */
{SPACE} { yycolumn += 1; }       /* 遇到空格，长度为 1 */
. {
    errors++;
    printf("Error type A at Line %d: Mysterious charaters \'%s\'.\n", yylineno, yytext);
}                                /* 其他不匹配的终结符，报错 */
%% 
