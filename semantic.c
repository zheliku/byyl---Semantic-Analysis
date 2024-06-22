#include "semantic.h"
#include "tree.h"
#include "log.h"

int anonymous = 0;
extern HashTable *symbolTable;
Symbol *currentFunction = NULL;
functionSm * functions_sm=NULL;

void checkFunction()
{
    functionSm *t=functions_sm;
    while(t!=NULL)
    {
        if(t->dingyi==false)
            reportError(SemanticError, 18, t->line_num, "Undefined function!");
        t=t->next;
    }
}

bool insertFunction(functionSm* function_add)
{
//    printf("%s",function_add->name);
//    printf("\n\n!!%d!!\n\n",function_add->line_num);
    if(functions_sm==NULL){
        functions_sm=(functionSm*) malloc(sizeof(functionSm));
        functions_sm->name=function_add->name;
        functions_sm->dingyi=false;
        functions_sm->line_num=function_add->line_num;
        functions_sm->next=NULL;
    }else
    {
        functionSm *p,*t;
        p=(functionSm *)malloc(sizeof(functionSm));
        p->name=function_add->name;
        p->dingyi=false;
        p->line_num=function_add->line_num;
        t=functions_sm;
        while(t->next!=NULL){
            if(strcmp(t->name,function_add->name)==0)
            {
                reportError(SemanticError, 19, function_add->line_num, "Incomplete definition of function.");
            }
            t=t->next;
        }
        if(strcmp(t->name,function_add->name)==0)
        {
            reportError(SemanticError, 19, function_add->line_num, "Incomplete definition of function.");
        }
        t->next=p;
        t->next->next=NULL;
    }

    return true;
}

void printFunctions()
{
    functionSm *t=functions_sm;
    while(t!=NULL)
    {
        printf("%s\n",t->name);
        t=t->next;
    }
}

bool checkNode(Node *node, Types type) {
	if (node == NULL) {
		addLogNullNode(type);
		return false;
	}
	else if (node->type != type) {
		addLogTypeDismatch(type);
		return false;
	}
	addLogStartDealing(type);
	return true;
}

bool hasBrothers(Node *node, int n, ...) {
	va_list vaList;
	va_start(vaList, n);
	Types next;
	for (int i = 0; i < n; ++i) {
		next = va_arg(vaList, Types);
		if (node == NULL || next != node->type) return false;
		node = node->brother;
	}
	return node == NULL;
}

/****handlers****/

bool dealWithProgram(Node *node) {
	if (checkNode(node, _Program) == false) return false;
	Node *c = node->child;
	if (c == NULL) {
		addLogNullChild(_Program);
		return false;
	}
	if (hasBrothers(c, 1, _ExtDefList)) {
		return dealWithExtDefList(c);
	}
	else {
		addLogWrongChild(_Program);
		return false;
	}
}

bool dealWithExtDefList(Node *node) {
	if (checkNode(node, _ExtDefList) == false) return false;
	Node *c = node->child;
	if (c == NULL) {
		addLogNullChild(_ExtDefList);
		return false;
	}
	if (hasBrothers(c, 1, _BLANK)) {
		addLogEmptyNode(_ExtDefList);
		return true;
	}
	while (c != NULL) {
		if (c->type == _ExtDef) {
			dealWithExtDef(c);
		}
		else if (c->type == _ExtDefList) {
			dealWithExtDefList(c);
		}
		else {
			addLogWrongChild(_ExtDefList);
		}
		c = c->brother;
	}
	return true;
}

bool dealWithExtDef(Node *node) {
	if (checkNode(node, _ExtDef) == false) return false;

	Node *c = node->child;
	if (c == NULL) {
		addLogNullChild(_ExtDef);
		return false;
	}
	if (hasBrothers(c, 2, _Specifier, _SEMI)) {
		//ExtDef := Specifier SEMI
		ValueTypes *type = (ValueTypes *) malloc(sizeof(ValueTypes));
		char **name = (char **) malloc(sizeof(char *));
		dealWithSpecifier(c, type, name);
		return true;
	}
	else if (hasBrothers(c, 3, _Specifier, _ExtDecList, _SEMI)) {
		//ExtDef := Specifier ExtDecList SEMI
		ValueTypes *type = (ValueTypes *) malloc(sizeof(ValueTypes));
		char **name = (char **) malloc(sizeof(char *));
		dealWithSpecifier(c, type, name);
		return dealWithExtDecList(c->brother, type, name);
	}
	else if (hasBrothers(c, 3, _Specifier, _FunDec, _CompSt)) {
		//ExtDef := Specifier FunDec CompSt
		ValueTypes *type = (ValueTypes *) malloc(sizeof(ValueTypes));
		char **name = (char **) malloc(sizeof(char *));
		dealWithSpecifier(c, type, name);
		Symbol *s = createSymbol();
		setSymbolType(s, FUNC_SYMBOL);
		setFuncReturnValue(s, *type, *name);
		currentFunction = s;
		dealWithFunDec(c->brother, s);
		if (!insert(symbolTable, s)) {
			reportError(SemanticError, 4, c->brother->lineNumber, "Duplicate definition of function.");
		}
        functionSm *t=functions_sm;
        while(t!=NULL && t->next!=NULL)
        {
            if(strcmp(t->name,s->name)==0)
            {
                t->dingyi=true;
            }
            t=t->next;
        }
		return dealWithCompSt(c->brother->brother);
	}else if(hasBrothers(c,3,_Specifier,_FunDec,_SEMI)){
        //ExtDef := Specifier FunDec SEMI
        functionSm *f = (functionSm *) malloc(sizeof(functionSm));
//        printf("\n\n%d\n\n",c->lineNumber);
        f->line_num=c->lineNumber;
//        printf("\n\n%d\n\n",f->line_num);
        ValueTypes *type = (ValueTypes *) malloc(sizeof(ValueTypes));
        char **name = (char **) malloc(sizeof(char *));
        dealWithSpecifier(c, type, name);
        Symbol *s = createSymbol();
        setSymbolType(s, FUNC_SYMBOL);
        setFuncReturnValue(s, *type, *name);
        f->next = NULL;
        dealWithFunDec(c->brother, s);
        f->name=s->name;
//        printf("name:%s\n",s->name);
        insertFunction(f);
    }
	else {
		addLogWrongChild(_ExtDef);
		return false;
	}
}

bool dealWithSpecifier(Node *node, ValueTypes *type, char **name) {
	if (checkNode(node, _Specifier) == false) return false;
	Node *c = node->child;
	if (c == NULL) {
		addLogNullChild(_Specifier);
		return false;
	}
	if (c->type == _TYPE) {
		return dealWithTYPE(c, type);
	}
	else if (c->type == _StructSpecifier) {
		//Specifier := StructSpecifier
		return dealWithStructSpecifier(c, type, name);
	}
	else {
		addLogWrongChild(_Specifier);
		return false;
	}
}

bool dealWithStructSpecifier(Node *node, ValueTypes *type, char **name) {
	if (checkNode(node, _StructSpecifier) == false) return false;
	Node *c = node->child;
	if (c == NULL) {
		addLogNullChild(_StructSpecifier);
		return false;
	}
	if (hasBrothers(c, 5, _STRUCT, _OptTag, _LC, _DefList, _RC)) {
		//StructSpecifier := STRUCT OptTag LC DefList RC
		*type = _STRUCT_TYPE_;
		dealWithOptTag(c->brother, name);
		Symbol *s = createSymbol();
		setSymbolName(s, *name); // TODO handle epsilon
		setSymbolType(s, STRUCT_TYPE_SYMBOL);
		if (!insert(symbolTable, s)) {
			reportError(SemanticError, 16, c->lineNumber, "This struct type name has been used");
		}
		return dealWithDefList(c->brother->brother->brother, s);
	}
	else if (hasBrothers(c, 2, _STRUCT, _Tag)) {
		//StructSpecifier := STRUCT Tag
		*type = _STRUCT_TYPE_;
		return dealWithTag(c->brother, name);
	}
	else {
		addLogWrongChild(_StructSpecifier);
		return false;
	}
}

bool dealWithOptTag(Node *node, char **name) {
	if (checkNode(node, _OptTag) == false) return false;

	Node *c = node->child;
	if (c == NULL) {
		addLogNullChild(_OptTag);
		return false;
	}
	if (hasBrothers(c, 1, _ID)) {
		if (c->idName != NULL) {
			*name = (char *) malloc(strlen(c->idName) + 1);
			strcpy(*name, c->idName);
			return true;
		}
		else {
			addLogInfo(SemanticAnalysisLog, "when dealing with OptTag, the idName is NULL.\n");
			return false;
		}
	}
	else if (hasBrothers(c, 1, _BLANK)) {
		char str[80];
		sprintf(str, "$%d", anonymous);
		anonymous++;
		*name = (char *) malloc(strlen(str) + 1);
		strcpy(*name, str);
		return true;
	}
	else {
		addLogWrongChild(_OptTag);
		return false;
	}
}

bool dealWithTag(Node *node, char **name) {
	if (checkNode(node, _Tag) == false) return false;
	Node *c = node->child;
	if (c == NULL) {
		addLogNullChild(_Tag);
		return false;
	}
	if (hasBrothers(c, 1, _ID)) {
		if (c->idName != NULL) {
			*name = (char *) malloc(strlen(c->idName) + 1);
			strcpy(*name, c->idName);
			if (!isContain(symbolTable, c->idName) || get(symbolTable, c->idName)->symbol_type != STRUCT_TYPE_SYMBOL) {
				reportError(SemanticError, 17, c->lineNumber, "This struct type is undefined");
			}
			return true;
		}
		else {
			addLogInfo(SemanticAnalysisLog, "when dealing with Tag, the idName is NULL.\n");
			return false;
		}
	}
	else {
		addLogWrongChild(_Tag);
		return false;
	}
}

bool dealWithTYPE(Node *node, ValueTypes *type) {
	if (checkNode(node, _TYPE) == false) return false;

	if (node->idName != NULL) {
		if (strcmp("int", node->idName) == 0) {
			*type = _INT_TYPE_;
		}
		else if (strcmp("float", node->idName) == 0) {
			*type = _FLOAT_TYPE_;
		}
		else {
			addLogInfo(SemanticAnalysisLog, "when dealing with TYPE, the type name is wrong.\n");
			return false;
		}
	}
	else {
		addLogInfo(SemanticAnalysisLog, "when dealing with TYPE, the idName is NULL.\n");
		return false;
	}

	return true;
}

bool dealWithVarDec(Node *node, Symbol *s) {
	if (checkNode(node, _VarDec) == false) return false;
	Node *c = node->child;
	if (c == NULL) {
		addLogNullChild(_VarDec);
		return false;
	}

	if (hasBrothers(c, 1, _ID)) {
		setSymbolName(s, c->idName);
		//这里应该对vardec的类型进行设置
		return true;
	}
	else if (hasBrothers(c, 4, _VarDec, _LB, _INT, _RB)) {
		if (s->symbol_type == 0) {
			setSymbolType(s, ARRAY_SYMBOL);
		}
		addArrayDimension(s, c->brother->brother->intValue);
		return dealWithVarDec(c, s);
	}
	else {
		addLogWrongChild(_VarDec);
		return false;
	}
}

bool dealWithExtDecList(Node *node, ValueTypes *type, char **name) {
	if (checkNode(node, _ExtDecList) == false) return false;
	Node *c = node->child;
	if (c == NULL) {
		addLogNullChild(_ExtDecList);
		return false;
	}
	if (hasBrothers(c, 3, _VarDec, _COMMA, _ExtDecList)) {
		Symbol *s = createSymbol();
		dealWithVarDec(c, s);
		if (s->symbol_type == ARRAY_SYMBOL) {
			setArrayType(s, *type, *name);
			//insert(symbolTable, s);
		}
		else {
			if (*type == _INT_TYPE_) {
				setSymbolType(s, INT_SYMBOL);
				//insert(symbolTable, s);
			}
			else if (*type == _FLOAT_TYPE_) {
				setSymbolType(s, FLOAT_SYMBOL);
				//insert(symbolTable, s);
			}
			else if (*type == _STRUCT_TYPE_) {
				setSymbolType(s, STRUCT_VAL_SYMBOL);
				setStructValueType(s, *name);
				//insert(symbolTable, s);
			}
			else {
				addLogInfo(SemanticAnalysisLog, "when dealing with ExtDecList, VarDec type is wrong.\n");
				return false;
			}
		}
		if (!insert(symbolTable, s)) {
			reportError(SemanticError, 3, c->lineNumber, "Duplicate variable.");
		}
		return dealWithExtDecList(c->brother->brother, type, name);
	}
	else if (hasBrothers(c, 1, _VarDec)) {
		Symbol *s = createSymbol();
		dealWithVarDec(c, s);
		if (s->symbol_type == ARRAY_SYMBOL) {
			setArrayType(s, *type, *name);
			//insert(symbolTable, s);
		}
		else {
			if (*type == _INT_TYPE_) {
				setSymbolType(s, INT_SYMBOL);
				//insert(symbolTable, s);
			}
			else if (*type == _FLOAT_TYPE_) {
				setSymbolType(s, FLOAT_SYMBOL);
				//insert(symbolTable, s);
			}
			else if (*type == _STRUCT_TYPE_) {
				setSymbolType(s, STRUCT_VAL_SYMBOL);
				setStructValueType(s, *name);
				//insert(symbolTable, s);
			}
		}
		if (!insert(symbolTable, s)) {
			reportError(SemanticError, 3, c->lineNumber, "Duplicate variable.");
		}
		return true;
	}
	else {
		addLogWrongChild(_ExtDecList);
		return false;
	}
}

bool dealWithDefList(Node *node, Symbol *s) {
	if (checkNode(node, _DefList) == false) return false;
	Node *c = node->child;
	if (c == NULL) {
		addLogNullChild(_DefList);
		return false;
	}
	if (hasBrothers(c, 1, _BLANK)) {
		addLogEmptyNode(_DefList);
		return true;
	}
	while (c != NULL) {
		if (c->type == _DefList) {
			dealWithDefList(c, s);
		}
		else if (c->type == _Def) {
			dealWithDef(c, s);
		}
		else {
			addLogWrongChild(_DefList);
		}
		c = c->brother;
	}
	return true;
}

bool dealWithDef(Node *node, Symbol *s) {
	if (checkNode(node, _Def) == false) return false;

	Node *c = node->child;
	if (c == NULL) {
		addLogNullChild(_Def);
		return false;
	}
	if (hasBrothers(c, 3, _Specifier, _DecList, _SEMI)) {
		//Def := Specifier DecList SEMI
		ValueTypes *type = (ValueTypes *) malloc(sizeof(ValueTypes));
		char **name = (char **) malloc(sizeof(char *));
		dealWithSpecifier(c, type, name);
		return dealWithDecList(c->brother, s, type, name);
	}
	else {
		addLogWrongChild(_Def);
		return false;
	}
}

bool dealWithDecList(Node *node, Symbol *s, ValueTypes *type, char **name) {
	if (checkNode(node, _DecList) == false) return false;
	Node *c = node->child;
	if (c == NULL) {
		addLogNullChild(_DecList);
		return false;
	}
	if (hasBrothers(c, 3, _Dec, _COMMA, _DecList)) {
		Symbol *ss = createSymbol();
		dealWithDec(c, s, ss);
		if (ss->symbol_type == ARRAY_SYMBOL) {
			setArrayType(ss, *type, *name);
			//insert(symbolTable, ss);
		}
		else {
			if (*type == _INT_TYPE_) {
				setSymbolType(ss, INT_SYMBOL);
				//insert(symbolTable, ss);
			}
			else if (*type == _FLOAT_TYPE_) {
				setSymbolType(ss, FLOAT_SYMBOL);
				//insert(symbolTable, ss);
			}
			else if (*type == _STRUCT_TYPE_) {
				setSymbolType(ss, STRUCT_VAL_SYMBOL);
				setStructValueType(ss, *name);
				//insert(symbolTable, ss);
			}
		}
		if (s != NULL && s->symbol_type == STRUCT_TYPE_SYMBOL) {
			if (insert(symbolTable, ss)) {
				addStructTypeField(s, ss->name);
			}
			else {
				reportError(SemanticError, 15, c->lineNumber, "Wrong field in struct definition.");
			}
		}
		else {
			if (!insert(symbolTable, ss)) {
				reportError(SemanticError, 3, c->lineNumber, "Duplicate variable.");
			}
		}
		return dealWithDecList(c->brother->brother, s, type, name);
	}
	else if (hasBrothers(c, 1, _Dec)) {
		Symbol *ss = createSymbol();
		dealWithDec(c, s, ss);
		if (ss->symbol_type == ARRAY_SYMBOL) {
			setArrayType(ss, *type, *name);
			//insert(symbolTable, ss);
		}
		else {
			if (*type == _INT_TYPE_) {
				setSymbolType(ss, INT_SYMBOL);
				//insert(symbolTable, ss);
			}
			else if (*type == _FLOAT_TYPE_) {
				setSymbolType(ss, FLOAT_SYMBOL);
				//insert(symbolTable, ss);
			}
			else if (*type == _STRUCT_TYPE_) {
				setSymbolType(ss, STRUCT_VAL_SYMBOL);
				setStructValueType(ss, *name);
				//insert(symbolTable, ss);
			}
		}
		if (s != NULL && s->symbol_type == STRUCT_TYPE_SYMBOL) {
			if (insert(symbolTable, ss)) {
				addStructTypeField(s, ss->name);
			}
			else {
				reportError(SemanticError, 15, c->lineNumber, "Wrong field in struct definition.");
			}
		}
		else {
			if (!insert(symbolTable, ss)) {
				reportError(SemanticError, 3, c->lineNumber, "Duplicate variable.");
			}
		}
		return true;
	}
	else {
		addLogWrongChild(_DecList);
		return false;
	}
}

bool dealWithDec(Node *node, Symbol *s, Symbol *field) {
	if (checkNode(node, _Dec) == false) return false;
	Node *c = node->child;
	if (c == NULL) {
		addLogNullChild(_Dec);
		return false;
	}

	if (hasBrothers(c, 3, _VarDec, _ASSIGNOP, _Exp)) {
		addLogInfo(SemanticAnalysisLog, "Going to handle Exp!!!.\n");
		if (s != NULL) {
			reportError(SemanticError, 15, c->lineNumber, "Cannot initialize a field in definition of a struct");
			return dealWithVarDec(c, field);
		}
		else {
			dealWithVarDec(c, field);
			//printf("dec:%s, type%d\n", field->name, field->symbol_type); //field的type设置的不正确
			ExpType *expType = (ExpType *) malloc(sizeof(ExpType));
			dealWithExp(c->brother->brother, expType);
			//printf("exp type: %d\n", expType->type);
		}
	}
	else if (hasBrothers(c, 1, _VarDec)) {
		return dealWithVarDec(c, field);
	}
	else {
		addLogWrongChild(_Dec);
		return false;
	}

	return true;
}

bool dealWithFunDec(Node *node, Symbol *s) {
	if (checkNode(node, _FunDec) == false) return false;
	Node *c = node->child;
	if (c == NULL) {
		addLogNullChild(_FunDec);
		return false;
	}

	if (hasBrothers(c, 4, _ID, _LP, _VarList, _RP)) {
		if (c->idName != NULL) {
			setSymbolName(s, c->idName);
			return dealWithVarList(c->brother->brother, s);
		}
		else {
			addLogInfo(SemanticAnalysisLog, "when dealing with FunDec, the idName is NULL.\n");
			return false;
		}
	}
	else if (hasBrothers(c, 3, _ID, _LP, _RP)) {
		if (c->idName != NULL) {
			setSymbolName(s, c->idName);
			return true;
		}
		else {
			addLogInfo(SemanticAnalysisLog, "when dealing with FunDec, the idName is NULL.\n");
			return false;
		}
	}
	return true;
}

bool dealWithVarList(Node *node, Symbol *s) {
	if (checkNode(node, _VarList) == false) return false;
	Node *c = node->child;
	if (c == NULL) {
		addLogNullChild(_VarList);
		return false;
	}

	if (hasBrothers(c, 3, _ParamDec, _COMMA, _VarList)) {
		Symbol *argument = createSymbol();
		dealWithParamDec(c, argument);
		insert(symbolTable, argument);
		addFuncArgument(s, argument->name);
		return dealWithVarList(c->brother->brother, s);
	}
	else if (hasBrothers(c, 1, _ParamDec)) {
		Symbol *argument = createSymbol();
		dealWithParamDec(c, argument);
		insert(symbolTable, argument);
		addFuncArgument(s, argument->name);
		return true;
	}
	else {
		addLogWrongChild(_VarList);
		return false;
	}
}

bool dealWithParamDec(Node *node, Symbol *s) {
	if (checkNode(node, _ParamDec) == false) return false;
	Node *c = node->child;
	if (c == NULL) {
		addLogNullChild(_ParamDec);
		return false;
	}

	if (hasBrothers(c, 2, _Specifier, _VarDec)) {
		ValueTypes *type = (ValueTypes *) malloc(sizeof(ValueTypes));
		char **name = (char **) malloc(sizeof(char *));
		dealWithSpecifier(c, type, name);
		dealWithVarDec(c->brother, s);
		if (s->symbol_type == ARRAY_SYMBOL) {
			setArrayType(s, *type, *name);
		}
		else {
			if (*type == _INT_TYPE_) {
				setSymbolType(s, INT_SYMBOL);
			}
			else if (*type == _FLOAT_TYPE_) {
				setSymbolType(s, FLOAT_SYMBOL);
			}
			else if (*type == _STRUCT_TYPE_) {
				setSymbolType(s, STRUCT_VAL_SYMBOL);
				setStructValueType(s, *name);
			}
		}
	}
	else {
		addLogWrongChild(_ParamDec);
		return false;
	}
	return true;
}

bool dealWithCompSt(Node *node) {
	if (checkNode(node, _CompSt) == false) return false;
	Node *c = node->child;
	if (c == NULL) {
		addLogNullChild(_CompSt);
		return false;
	}

	if (hasBrothers(c, 4, _LC, _DefList, _StmtList, _RC)) {
		dealWithDefList(c->brother, NULL);
		dealWithStmtList(c->brother->brother);
		return true;
	}
	else {
		addLogWrongChild(_CompSt);
		return false;
	}
}

bool dealWithStmtList(Node *node) {
	if (checkNode(node, _StmtList) == false) return false;
	Node *c = node->child;
	if (c == NULL) {
		addLogNullChild(_StmtList);
		return false;
	}
	if (hasBrothers(c, 1, _BLANK)) {
		addLogEmptyNode(_StmtList);
		return true;
	}
	while (c != NULL) {
		if (c->type == _StmtList) {
			dealWithStmtList(c);
		}
		else if (c->type == _Stmt) {
			dealWithStmt(c);
		}
		else {
			addLogWrongChild(_StmtList);
		}
		c = c->brother;
	}
	return true;
}

bool dealWithStmt(Node *node) {
	if (checkNode(node, _Stmt) == false) return false;
	Node *c = node->child;
	if (c == NULL) {
		addLogNullChild(_Stmt);
		return false;
	}

	if (hasBrothers(c, 2, _Exp, _SEMI)) {
		//Stmt := Exp SEMI
		addLogInfo(SemanticAnalysisLog, "Going to handle Exp.\n");
		ExpType *expType = (ExpType *) malloc(sizeof(ExpType));
		dealWithExp(c, expType);
		return true;
	}
	else if (hasBrothers(c, 1, _CompSt)) {
		return dealWithCompSt(c);
	}
	else if (hasBrothers(c, 3, _RETURN, _Exp, _SEMI)) {
		//Stmt := RETURN Exp SEMI
		addLogInfo(SemanticAnalysisLog, "Going to handle Exp.\n");
		ExpType *expType = (ExpType *) malloc(sizeof(ExpType));
		dealWithExp(c->brother, expType);
		//对EXP返回的类型进行检测
		if (currentFunction == NULL || currentFunction->symbol_type != FUNC_SYMBOL) {
			addLogInfo(SemanticAnalysisLog, "Handle a return expression with incorrect function");
			return false;
		}
		ValueTypes retType = currentFunction->func_content->retType;
		if (expType->type != retType) {
			reportError(SemanticError, 8, c->lineNumber, "return value mismatch");
			return false;
		}
		else if (expType->type == STRUCT_VAL_SYMBOL) {
			StructTypeContent *s1 = get(symbolTable, expType->typeName)->struct_def;
			StructTypeContent *s2 = get(symbolTable, currentFunction->func_content->typeName)->struct_def;
			if (!structTypeEqual(s1, s2)) {
				reportError(SemanticError, 8, c->lineNumber, "return value mismatch");
				return false;
			}
		}
		return true;
	}
		//注意条件表达式只能是INT
	else if (hasBrothers(c, 5, _WHILE, _LP, _Exp, _RP, _Stmt)) {
		ExpType *expType = (ExpType *) malloc(sizeof(ExpType));
		dealWithExp(c->brother->brother, expType);
		if (expType->type != _INT_TYPE_) {
			addLogInfo(SemanticAnalysisLog, "Exp must be int");
			//return false;
		}
		return dealWithStmt(c->brother->brother->brother->brother);
	}
	else if (hasBrothers(c, 7, _IF, _LP, _Exp, _RP, _Stmt, _ELSE, _Stmt)) {
		dealWithStmt(c->brother->brother->brother->brother);
		dealWithStmt(c->brother->brother->brother->brother->brother->brother);
		ExpType *expType = (ExpType *) malloc(sizeof(ExpType));
		dealWithExp(c->brother->brother, expType);
		if (expType->type != _INT_TYPE_) {
			addLogInfo(SemanticAnalysisLog, "Exp must be int");
			return false;
		}
		return true;
	}
	else if (hasBrothers(c, 5, _IF, _LP, _Exp, _RP, _Stmt)) {
		ExpType *expType = (ExpType *) malloc(sizeof(ExpType));
		dealWithExp(c->brother->brother, expType);
		if (expType->type != _INT_TYPE_) {
			addLogInfo(SemanticAnalysisLog, "Exp must be int");
			//return false;
		}
		return dealWithStmt(c->brother->brother->brother->brother);
	}
	else {
		addLogWrongChild(_Stmt);
		return false;
	}
}

bool dealWithExp(Node *node, ExpType *expType) {
	if (checkNode(node, _Exp) == false) return false;

	Node *c = node->child;
	//错误处理
	if (c == NULL) {
		addLogNullChild(_Exp);
		return false;
	}
		// case : EXP->ID
	else if (hasBrothers(c, 1, _ID)) {
		//printf("Exp->id\n");
		char *idName = c->idName;
		if (!isContain(symbolTable, idName)) {
			reportError(SemanticError, 1, c->lineNumber, "Undefined ID");
			return false;
		}
		else {
			Symbol *s = get(symbolTable, idName);
			switch (s->symbol_type) {
				case INT_SYMBOL:
					expType->type = _INT_TYPE_;
					expType->leftValue = true;
					break;
				case FLOAT_SYMBOL:
					expType->type = _FLOAT_TYPE_;
					expType->leftValue = true;
					break;
				case ARRAY_SYMBOL:
					expType->type = _ARRAY_TYPE_;
					expType->leftValue = true;
					expType->arrayContent = s->array_content;
					break;
				case STRUCT_VAL_SYMBOL:
					expType->type = _STRUCT_TYPE_;
					expType->leftValue = true;
					expType->typeName = s->struct_value->typeName;
					break;
				case STRUCT_TYPE_SYMBOL:
					reportError(SemanticError, 1, c->lineNumber, "Illegal ID");
					return false;
				case FUNC_SYMBOL:
					break;
			}
			return true;
		}
	}
		// case : EXP->INT
	else if (hasBrothers(c, 1, _INT)) {
		//printf("Exp -> int\n");
		expType->type = _INT_TYPE_;
		expType->leftValue = false;
		return true;
	}
		// case : EXP->FLOAT
	else if (hasBrothers(c, 1, _FLOAT)) {
		expType->type = _FLOAT_TYPE_;
		expType->leftValue = false;
		return true;
	}
		// case : EXP->EXP ASSIGNOP EXP
	else if (hasBrothers(c, 3, _Exp, _ASSIGNOP, _Exp)) {
		//printf("Exp -> Exp = Exp\n");
		ExpType *type1 = (ExpType *) malloc(sizeof(ExpType));
		ExpType *type2 = (ExpType *) malloc(sizeof(ExpType));
		dealWithExp(c, type1);
		dealWithExp(c->brother->brother, type2);
		bool legalExp = true;
		if (!type1->leftValue) {
			reportError(SemanticError, 6, c->lineNumber, "Expression cannot be right value");
			legalExp = false;
		}
		//check type
		if (type1->type == _ARRAY_TYPE_ || type2->type == _ARRAY_TYPE_) {
			reportError(SemanticError, 5, c->lineNumber, "Wrong use of array");
			legalExp = false;
		}
		else if (!expTpyeEqual(type1, type2)) {
			reportError(SemanticError, 5, c->lineNumber, "Type mismatch");
			legalExp = false;
		}
		if (!legalExp) {
			return false;
		}
		//pass
		expType->leftValue = false;
		expType->type = type1->type;
		expType->typeName = type1->typeName;
		return true;
	}
		// case : EXP -> EXP PLUS EXP
		//        Exp -> Exp MINUS Exp
		//        Exp -> Exp STAR Exp
		//        Exp -> Exp DIV Exp
		//        Exp -> Exp RELOP Exp
	else if (hasBrothers(c, 3, _Exp, _PLUS, _Exp) ||
	         hasBrothers(c, 3, _Exp, _MINUS, _Exp) ||
	         hasBrothers(c, 3, _Exp, _STAR, _Exp) ||
	         hasBrothers(c, 3, _Exp, _DIV, _Exp) ||
	         hasBrothers(c, 3, _Exp, _RELOP, _Exp)) {
		//printf("EXP -> EXP + EXP\n");
		ExpType *type1 = (ExpType *) malloc(sizeof(ExpType));
		ExpType *type2 = (ExpType *) malloc(sizeof(ExpType));
		dealWithExp(c, type1);
		dealWithExp(c->brother->brother, type2);
		//check type
		//算式中，两边不能是数组和结构体
		if (type1->type == _ARRAY_TYPE_ || type2->type == _ARRAY_TYPE_) {
			reportError(SemanticError, 7, c->lineNumber, "Wrong use of array");
			return false;
		}
		if (type1->type == _STRUCT_TYPE_ || type2->type == _STRUCT_TYPE_) {
			reportError(SemanticError, 7, c->lineNumber, "Wrong use of struct");
			return false;
		}
		else if (!expTpyeEqual(type1, type2)) {
			reportError(SemanticError, 7, c->lineNumber, "Type mismatch");
			return false;
		}
		//pass
		expType->leftValue = false;
		expType->type = type1->type;
		return true;
	}
		// case : EXP -> EXP AND EXP
		//        EXP -> EXP OR EXP
	else if (hasBrothers(c, 3, _Exp, _AND, _Exp) ||
	         hasBrothers(c, 3, _Exp, _OR, _Exp)) {
		//printf("EXP->EXP && EXP\n");
		ExpType *type1 = (ExpType *) malloc(sizeof(ExpType));
		ExpType *type2 = (ExpType *) malloc(sizeof(ExpType));
		dealWithExp(c, type1);
		dealWithExp(c->brother->brother, type2);
		//check type
		//‘与’运算中，两个操作数都要是INT
		if (type1->type != _INT_TYPE_ || type2->type != _INT_TYPE_) {
			reportError(SemanticError, 7, c->lineNumber, "Type mismatch");
			return false;
		}
		//pass
		expType->leftValue = false;
		expType->type = _INT_TYPE_;
		return true;
	}
		// case : EXP -> LP EXP RP
	else if (hasBrothers(c, 3, _LP, _Exp, _RP)) {
		//printf("Exp->(Exp)\n");
		return dealWithExp(c->brother, expType);
	}
		// case : EXP -> MINUS EXP
	else if (hasBrothers(c, 2, _MINUS, _Exp)) {
		ExpType *childType = (ExpType *) malloc(sizeof(ExpType));
		dealWithExp(c->brother, childType);
		if (childType->type != _INT_TYPE_ && childType->type != _FLOAT_TYPE_) {
			reportError(SemanticError, 7, c->lineNumber, "Type mismatch"); //取负操作符的操作数只能是int或者float
			return false;
		}
		//pass
		expType->type = childType->type;
		expType->leftValue = false;
		return true;
	}
		// case : EXP -> NOT EXP
	else if (hasBrothers(c, 2, _NOT, _Exp) && c->brother->brother == NULL) {
		ExpType *childType = (ExpType *) malloc(sizeof(ExpType));
		dealWithExp(c->brother, childType);
		if (childType->type != _INT_TYPE_) {
			reportError(SemanticError, 7, c->lineNumber, "Type mismatch"); //NOT操作符的操作数只能是int
			return false;
		}
		//pass
		expType->type = _INT_TYPE_;
		expType->leftValue = false;
		return true;
	}
		// case : EXP -> EXP DOT ID
	else if (hasBrothers(c, 3, _Exp, _DOT, _ID)) {
		ExpType *childType = (ExpType *) malloc(sizeof(ExpType));
		dealWithExp(c, childType);
		if (childType->type != _STRUCT_TYPE_) {
			reportError(SemanticError, 13, c->lineNumber, "Type mismatch"); //DOT操作符的左操作数只能是struct
			return false;
		}
		// check whether id is the field of struct
		StructTypeContent *s = get(symbolTable, childType->typeName)->struct_def;
		char *fieldName = c->brother->brother->idName;
		if (!isContain(symbolTable, fieldName)) {
			reportError(SemanticError, 14, c->lineNumber, "Undefined field");
			return false;
		}
		if (!isField(s, fieldName)) {
			reportError(SemanticError, 14, c->lineNumber, "Undefined field"); //未定义的域
			return false;
		}
		//pass
		//int,float,struct可以作为左值, array不行
		Symbol *fs = get(symbolTable, fieldName);
		switch (fs->symbol_type) {
			case INT_SYMBOL:
				expType->type = _INT_TYPE_;
				expType->leftValue = true;
				break;
			case FLOAT_SYMBOL:
				expType->type = _FLOAT_TYPE_;
				expType->leftValue = true;
				break;
			case ARRAY_SYMBOL:
				expType->type = _ARRAY_TYPE_;
				expType->leftValue = false;
				expType->arrayContent = fs->array_content;
				break;
			case STRUCT_VAL_SYMBOL:
				expType->type = _STRUCT_TYPE_;
				expType->leftValue = true;
				expType->typeName = fs->struct_value->typeName;
				break;
			case STRUCT_TYPE_SYMBOL:
			case FUNC_SYMBOL:
				break;
		}
		return true;
	}
		//case: EXP-> EXP LB EXP RB
	else if (hasBrothers(c, 4, _Exp, _LB, _Exp, _RB)) {
		Node *exp1 = c;
		Node *exp2 = c->brother->brother;
		ExpType *type1 = (ExpType *) malloc(sizeof(ExpType));
		ExpType *type2 = (ExpType *) malloc(sizeof(ExpType));
		dealWithExp(exp1, type1);
		dealWithExp(exp2, type2);
		bool typeCheck = true;
		if (type1->type != _ARRAY_TYPE_) //只有数组可以取索引
		{
			reportError(SemanticError, 10, c->lineNumber, "Illegal array");
			typeCheck = false;
		}
		if (type2->type != _INT_TYPE_) //索引只能是int
		{
			reportError(SemanticError, 12, c->lineNumber, "Array index must be an integer");
			typeCheck = false;
		}
		if (!typeCheck) {
			return false;
		}
		//数组和索引都是合法的话，应该设置这个表达式的类型
		//特别注意，如果是1维数组的索引，返回的应该是基类型
		if (type1->arrayContent->dimensions == 1) {
			//一维数组
			expType->type = type1->arrayContent->type;
			expType->typeName = type1->arrayContent->typeName;
			expType->leftValue = true;
			return true;
		}
		else {
			//多维数组
			ArrayContent *ac = (ArrayContent *) malloc(sizeof(ArrayContent));
			ac->dimensions = type1->arrayContent->dimensions - 1;
			ac->size = (int *) malloc(sizeof(int) * ac->dimensions);
			for (int i = 0; i < ac->dimensions; i++) {
				ac->size[i] = type1->arrayContent->size[i + 1];
			}
			ac->type = type1->arrayContent->type;
			ac->typeName = type1->arrayContent->typeName;
			expType->type = _ARRAY_TYPE_;
			expType->arrayContent = ac;
			expType->leftValue = false;
			return true;
		}
	}
	// case : EXP->ID LP RP 无参函数调用
	if (hasBrothers(c, 3, _ID, _LP, _RP)) {
		//printf("function call2 @ line: %d\n", c->lineNumber);
		char *funcName = c->idName;
		if (!isContain(symbolTable, funcName)) {
			reportError(SemanticError, 2, c->lineNumber, "Undefined function");
			return false;
		}
		Symbol *s = get(symbolTable, funcName);
		if (s->symbol_type != FUNC_SYMBOL) {
			reportError(SemanticError, 11, c->lineNumber, "It is not callable");
			return false;
		}
		if (s->func_content->arguments != NULL) {
			reportError(SemanticError, 9, c->lineNumber, "Arguments mismatch");
			return false;
		}

		expType->type = s->func_content->retType;
		expType->typeName = s->func_content->typeName;
		return true;
	}
	// case : EXP -> ID LP ARGS RP 有参函数调用
	if (hasBrothers(c, 4, _ID, _LP, _Args, _RP)) {
		//printf("function call @ line: %d\n", c->lineNumber);
		char *funcName = c->idName;
		if (!isContain(symbolTable, funcName)) {
			reportError(SemanticError, 1, c->lineNumber, "Undefined ID");
			return false;
		}
		Symbol *s = get(symbolTable, funcName);
		if (s->symbol_type != FUNC_SYMBOL) {
			reportError(SemanticError, 11, c->lineNumber, "It is not callable");
			return false;
		}
		//要检查args是否匹配
		Argument *arguments = s->func_content->arguments; //原函数的参数列表
		ParaType *parameters = (ParaType *) malloc(sizeof(ParaType));
		dealWithArgs(c->brother->brother, parameters);
		parameters = parameters->next; //指向第一个参数
		if (argsMatch(arguments, parameters)) {
			expType->type = s->func_content->retType;
			expType->typeName = s->func_content->typeName;
			expType->leftValue = false;
			return true;
		}
		else {
			reportError(SemanticError, 9, c->lineNumber, "Arguments mismatch");
			return false;
		}
	}
	return true;
}

//处理实参
// 1)检查实参中的变量是否定义过
// 2)返回一个parameter list，和原函数的arg list进行比较
bool dealWithArgs(Node *node, ParaType *parameters) {
	//printf("Handle args\n");
	//printf("Line:%d\n", node->lineNumber);
	if (node == NULL || node->type != _Args) {
		addLogInfo(SemanticAnalysisLog, "Error when dealing with args.");
	}

	Node *c = node->child;
	//case: ARGS->EXP
	if (hasBrothers(c, 1, _Exp)) {
		ExpType *expType = (ExpType *) malloc(sizeof(ExpType));
		dealWithExp(c, expType);
		ParaType *paraType = (ParaType *) malloc(sizeof(ParaType));
		paraType->type = expType->type;
		paraType->arrayContent = expType->arrayContent;
		paraType->typeName = expType->typeName;
		parameters->next = paraType;
		return true;
	}
		//case: ARGS->EXP COMMA ARGS
	else if (hasBrothers(c, 3, _Exp, _COMMA, _Args)) {
		//printf("Case args->exp, args\n");
		ExpType *expType = (ExpType *) malloc(sizeof(ExpType));
		dealWithExp(c, expType); //问题在这里
		ParaType *paraType = (ParaType *) malloc(sizeof(ParaType));
		paraType->type = expType->type;
		paraType->arrayContent = expType->arrayContent;
		paraType->typeName = expType->typeName;
		//处理剩余的args
		dealWithArgs(c->brother->brother, paraType);
		parameters->next = paraType;
		return true;
	}
	return true;
}
