#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#ifndef __TREE_H_
#define __TREE_H_

extern const char *const TYPES_NAME_TABLE[];

typedef enum Types {
	/* 0 Blank */
	_BLANK,
	/* 1 Tokens */
	_SEMI,
	_COMMA,
	_ASSIGNOP,
	_RELOP,
	_PLUS,
	_MINUS,
	_STAR,
	_DIV,
	_AND,
	_OR,
	_DOT,
	_NOT,
	_TYPE,
	_LP,
	_RP,
	_LB,
	_RB,
	_LC,
	_RC,
	_STRUCT,
	_RETURN,
	_IF,
	_ELSE,
	_WHILE,
	_ID,
	_INT,
	_FLOAT,
	/* 2 High-level Definitions */
	_Program,
	_ExtDefList,
	_ExtDef,
	_ExtDecList,
	/* 3 Specifiers */
	_Specifier,
	_StructSpecifier,
	_OptTag,
	_Tag,
	/* 4 Declarators */
	_VarDec,
	_FunDec,
	_VarList,
	_ParamDec,
	/* 5 Statements */
	_CompSt,
	_StmtList,
	_Stmt,
	/* 6 Local Definitions */
	_DefList,
	_Def,
	_DecList,
	_Dec,
	/* 7 Expressions */
	_Exp,
	_Args
} Types;

typedef struct Node {
	Types type;
	union {
		char *idName;
		int intValue;
		float floatValue;
	};
	int lineNumber;
	struct Node *child;
	struct Node *brother;
} Node;

const char *typeToString(Types type);                   // 得到结点的名字

Node *createNode(Types type, int lineNum);              // 新建结点

void addChild(Node *father, Node *child);               // 为一个子节点插入父节点

Node *insertNode(Types type, int lineNum, int n, ...);  // 为多个子节点插入父节点

void printNode(Node *node, FILE *f);                             // 打印单个节点

void printTree(Node *node, int depth, FILE *f);                  // 打印语法树

void destroyTree(Node *node);                           // 销毁语法树

#endif
