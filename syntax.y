%{
#include <stdio.h>
#include "lex.yy.c"
#include "tree.h"

Node* root = NULL;

#ifndef YYSTYPE
#define YYSTYPE Node*
#endif
void yyerror(const char *s);
void my_yyerror(const char* msg);

int error_line = -1;
int pre_error_line = -1;
int syntax_correct = 1;

%}
/*定义类型*/

/*定义tokens*/
%token INT
%token FLOAT
%token ID
%token SEMI
%token COMMA
%token ASSIGNOP
%token RELOP
%token PLUS MINUS STAR DIV
%token AND OR NOT
%token DOT
%token TYPE
%token LP RP LB RB LC RC
%token STRUCT
%token RETURN
%token IF
%token ELSE
%token WHILE
/*定义非终结符号*/

/*结合规则*/
%right ASSIGNOP
%left OR
%left AND
%nonassoc RELOP
%left PLUS MINUS
%left STAR DIV
%right NAGATE NOT
%right DOT LP LB RP RB
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE


/*定义产生式规则*/
%%
Program : ExtDefList { $$ = insertNode(_Program, $1->lineNumber, 1, $1); root = $$; }
    ;
ExtDefList : /*empty*/ { $$ = insertNode(_ExtDefList, yylineno, 0); }
    | ExtDef ExtDefList { $$ = insertNode(_ExtDefList, $1->lineNumber, 2, $1, $2); }
    ;
ExtDef : Specifier ExtDecList SEMI { $$ = insertNode(_ExtDef,$1->lineNumber, 3, $1, $2, $3); }
    | Specifier SEMI   { $$ = insertNode(_ExtDef, $1->lineNumber, 2, $1, $2); }
    | Specifier FunDec CompSt   { $$ = insertNode(_ExtDef, $1->lineNumber, 3, $1, $2, $3); }
    | Specifier FunDec SEMI   { $$ = insertNode(_ExtDef, $1->lineNumber, 3, $1, $2, $3); }
    ;
ExtDecList : VarDec { $$ = insertNode(_ExtDecList, $1->lineNumber, 1, $1); }
    | VarDec COMMA ExtDecList   { $$ = insertNode(_ExtDecList, $1->lineNumber, 3, $1, $2, $3); }
    ;
Specifier : TYPE    { $$ = insertNode(_Specifier, $1->lineNumber, 1, $1); }
    | StructSpecifier   { $$ = insertNode(_Specifier, $1->lineNumber, 1, $1); }
    ;
StructSpecifier : STRUCT OptTag LC DefList RC   { $$ = insertNode(_StructSpecifier, $1->lineNumber, 5, $1, $2, $3, $4, $5); }
    | STRUCT Tag    { $$ = insertNode(_StructSpecifier, $1->lineNumber, 2, $1, $2); }
    ;
OptTag : /*empty*/  { $$ = insertNode(_OptTag, yylineno, 0); }
    | ID    { $$ = insertNode(_OptTag, $1->lineNumber, 1, $1); }
    ;
Tag : ID    { $$ = insertNode(_Tag, $1->lineNumber, 1, $1); }
    ;
VarDec : ID { $$ = insertNode(_VarDec, $1->lineNumber, 1, $1); }
    | VarDec LB INT RB  { $$ = insertNode(_VarDec, $1->lineNumber, 4, $1, $2, $3, $4); }
    ;
FunDec : ID LP VarList RP   { $$ = insertNode(_FunDec, $1->lineNumber, 4, $1, $2, $3, $4); }
    | ID LP RP  { $$ = insertNode(_FunDec, $1->lineNumber, 3, $1, $2, $3); }
    ;
VarList : ParamDec COMMA VarList    { $$ = insertNode(_VarList, $1->lineNumber, 3, $1, $2, $3); }
    | ParamDec  { $$ = insertNode(_VarList, $1->lineNumber, 1, $1); }
    ;
ParamDec : Specifier VarDec { $$ = insertNode(_ParamDec, $1->lineNumber, 2, $1, $2); }
    ;
CompSt : LC DefList StmtList RC { $$ = insertNode(_CompSt, $1->lineNumber, 4, $1, $2, $3, $4); }
    ;
StmtList : /*empty*/    { $$ = insertNode(_StmtList, yylineno, 0); }
    | Stmt StmtList { $$ = insertNode(_StmtList, $1->lineNumber, 2, $1, $2); }
    ;
Stmt : Exp SEMI { $$ = insertNode(_Stmt, $1->lineNumber, 2, $1, $2); }
    | CompSt { $$ = insertNode(_Stmt, $1->lineNumber, 1, $1); }
    | RETURN Exp SEMI   { $$ = insertNode(_Stmt, $1->lineNumber, 3, $1, $2, $3); }

    | IF LP Exp RP Stmt { $$ = insertNode(_Stmt, $1->lineNumber, 5, $1, $2, $3, $4, $5); }

    | IF LP Exp RP Stmt ELSE Stmt { $$ = insertNode(_Stmt, $1->lineNumber, 7, $1, $2, $3, $4, $5, $6, $7); }

    | WHILE LP Exp RP Stmt  { $$ = insertNode(_Stmt, $1->lineNumber, 5, $1, $2, $3, $4, $5); }
    ;
DefList : /*empty*/ { $$ = insertNode(_DefList, yylineno, 0); }
    | Def DefList   { $$ = insertNode(_DefList, $1->lineNumber, 2, $1, $2); }
    ;
Def : Specifier DecList SEMI    { $$ = insertNode(_Def, $1->lineNumber, 3, $1, $2, $3); }
    | Specifier error SEMI { $$ = insertNode(_Def, $1->lineNumber, 3, $1, $2, $3); error_line = $2->lineNumber; my_yyerror("declarators definition error."); }
    | error SEMI { $$ = insertNode(_Def, yylineno, 2, $1, $2); error_line = $1->lineNumber; my_yyerror("definition error."); }
    ;
DecList : Dec   { $$ = insertNode(_DecList, $1->lineNumber, 1, $1); }
    | Dec COMMA DecList { $$ = insertNode(_DecList, $1->lineNumber, 3, $1, $2, $3); }
    ;
Dec : VarDec    { $$ = insertNode(_Dec, $1->lineNumber, 1, $1); }
    | VarDec ASSIGNOP Exp   { $$ = insertNode(_Dec, $1->lineNumber, 3, $1, $2, $3); }
    ;
Exp : Exp ASSIGNOP Exp  { $$ = insertNode(_Exp, $1->lineNumber, 3, $1, $2, $3); }
    | Exp AND Exp   { $$ = insertNode(_Exp, $1->lineNumber, 3, $1, $2, $3); }
    | Exp OR Exp    { $$ = insertNode(_Exp, $1->lineNumber, 3, $1, $2, $3); }
    | Exp RELOP Exp { $$ = insertNode(_Exp, $1->lineNumber, 3, $1, $2, $3); }
    | Exp PLUS Exp  { $$ = insertNode(_Exp, $1->lineNumber, 3, $1, $2, $3); }
    | Exp MINUS Exp { $$ = insertNode(_Exp, $1->lineNumber, 3, $1, $2, $3); }
    | Exp STAR Exp  { $$ = insertNode(_Exp, $1->lineNumber, 3, $1, $2, $3); }
    | Exp DIV Exp   { $$ = insertNode(_Exp, $1->lineNumber, 3, $1, $2, $3); }
    | LP Exp RP { $$ = insertNode(_Exp, $1->lineNumber, 3, $1, $2, $3); }
    | MINUS Exp { $$ = insertNode(_Exp, $1->lineNumber, 2, $1, $2); }
    | NOT Exp   { $$ = insertNode(_Exp, $1->lineNumber, 2, $1, $2); }
    | ID LP Args RP { $$ = insertNode(_Exp, $1->lineNumber, 4, $1, $2, $3, $4); }
    | ID LP RP  { $$ = insertNode(_Exp, $1->lineNumber, 3, $1, $2, $3); }
    | Exp LB Exp RB { $$ = insertNode(_Exp, $1->lineNumber, 4, $1, $2, $3, $4); }
    | Exp DOT ID    { $$ = insertNode(_Exp, $1->lineNumber, 3, $1, $2, $3); }
    | ID    { $$ = insertNode(_Exp, $1->lineNumber, 1, $1); }
    | INT   { $$ = insertNode(_Exp, $1->lineNumber, 1, $1); }
    | FLOAT { $$ = insertNode(_Exp, $1->lineNumber, 1, $1); }

    | Exp ASSIGNOP error { $$ = insertNode(_Exp, $1->lineNumber, 3, $1, $2, $3); error_line = $3->lineNumber; my_yyerror("something wrong with your expression"); }
    | Exp LB error RB { $$ = insertNode(_Exp, $1->lineNumber, 4, $1, $2, $3, $4); error_line = $3->lineNumber; my_yyerror("something wrong between \'[]\'"); }
    ;
Args : Exp COMMA Args   { $$ = insertNode(_Args, $1->lineNumber, 3, $1, $2, $3); }
    | Exp   { $$ = insertNode(_Args, $1->lineNumber, 1, $1); }
    ;

%%

void  yyerror(const char* msg) {
    //printf("Errir type B at line %d : %s\n", error_line, msg);
    //do nothing
}

void my_yyerror(const char* msg) {
    if(error_line == pre_error_line) {
        return;
    }
    printf("Error type B at Line %d : %s\n", error_line, msg);
    pre_error_line = error_line;
    syntax_correct = 0;
}
