#ifndef __SEMACTIC_H_
#define __SEMACTIC_H_

#include "hashTable.h"
#include "tree.h"
#include "symbol.h"
#include "stdarg.h"

void printFunctions();
void checkFunction();


bool checkNode(Node *node, Types type);

bool hasBrothers(Node *node, int n, ...);

bool dealWithProgram(Node *node);

bool dealWithExtDefList(Node *node);

bool dealWithExtDef(Node *node);

bool dealWithSpecifier(Node *node, ValueTypes *type, char **name);

bool dealWithTYPE(Node *node, ValueTypes *type);

bool dealWithStructSpecifier(Node *node, ValueTypes *type, char **name);

bool dealWithTag(Node *node, char **name);

bool dealWithOptTag(Node *node, char **name);

bool dealWithExtDecList(Node *node, ValueTypes *type, char **name);

bool dealWithVarDec(Node *node, Symbol *s);

bool dealWithDefList(Node *node, Symbol *s);

bool dealWithDef(Node *node, Symbol *s);

bool dealWithDecList(Node *node, Symbol *s, ValueTypes *type, char **name);

bool dealWithDec(Node *node, Symbol *s, Symbol *field);

bool dealWithFunDec(Node *node, Symbol *s);

bool dealWithVarList(Node *node, Symbol *s);

bool dealWithParamDec(Node *node, Symbol *s);

bool dealWithCompSt(Node *node);

bool dealWithStmtList(Node *node);

bool dealWithStmt(Node *node);

bool dealWithExp(Node *node, ExpType *expType);

bool dealWithArgs(Node *node, ParaType *parameters);

#endif
