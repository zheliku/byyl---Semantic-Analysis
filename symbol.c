#include "symbol.h"
#include "hashTable.h"
#include "stdbool.h"
#include <stdlib.h>
#include <string.h>

extern HashTable *symbolTable;

/**
 * 实现创建符号表项，申请空间，内容置为默认值
 * @return
 */
Symbol *createSymbol() {
	Symbol *s = (Symbol *) malloc(sizeof(Symbol));
	if (s == NULL) {
		return NULL;
	}
	s->next = NULL;
	s->name = NULL;
	s->symbol_type = 0;
	return s;
}

/**
 * 设置symbol的名称
 * @param s 符号表指针
 * @param name 要赋给符号表的名字
 * @return 返回布尔值表示是否成功
 */
bool setSymbolName(Symbol *s, char *name) {
	//前提：表项存在，未被赋名过
	if (s == NULL) {
		return false;
	}
	if (s->name != NULL) {
		return false;
	}
	//申请空间，申请成功后赋名
	s->name = (char *) malloc(sizeof(char) * strlen(name) + 1);
	if (s->name == NULL) {
		return false;
	}
	strcpy(s->name, name);
	return true;
}

/**
 * 设置symbol的类型
 * @param s 符号表指针
 * @param type 要赋给符号表的类型
 * @return 返回布尔值表示是否成功
 */
bool setSymbolType(Symbol *s, SymbolTypes type) {
	//前提：表项存在，未被赋类型过
	if (s == NULL) {
		return false;
	}
	if (s->symbol_type != 0) {
		//printf("When setting symbol(%s) type, this symbol already has a type %d.\n", s->name, s->symbol_type);
		return false;
	}
	//直接为表项赋类型
	s->symbol_type = type;
	//根据表项类型，为表项内容申请空间或赋默认值
	switch (s->symbol_type) {
		case INT_SYMBOL:
			s->int_content = (IntContent *) malloc(sizeof(IntContent));
			break;
		case FLOAT_SYMBOL:
			s->float_content = (FloatContent *) malloc(sizeof(FloatContent));
			break;
		case FUNC_SYMBOL:
			s->func_content = (FuncContent *) malloc(sizeof(FuncContent));
			s->func_content->retType = 0;
			s->func_content->arguments = NULL;
			s->func_content->typeName = NULL;
			break;
		case ARRAY_SYMBOL:
			s->array_content = (ArrayContent *) malloc(sizeof(ArrayContent));
			s->array_content->type = 0;
			s->array_content->dimensions = 0;
			s->array_content->typeName = NULL;
			s->array_content->size = NULL;
			break;
		case STRUCT_TYPE_SYMBOL:
			s->struct_def = (StructTypeContent *) malloc(sizeof(StructTypeContent));
			s->struct_def->fields = NULL;
			break;
		case STRUCT_VAL_SYMBOL:
			s->struct_value = (StructValueContent *) malloc(sizeof(StructValueContent));
			s->struct_value->typeName = NULL;
			break;
	}
	return true;
}

/**
 * 数组的维度+1，数组的维度数不能一次获取到，因此需要一个一个累加
 * @param s 符号表指针
 * @param size 新增维度的大小
 * @return 返回布尔值表示是否成功
 */
bool addArrayDimension(Symbol *s, int size) {
	//前提是表项存在且数组内容不是默认值
	if (s == NULL) {
		return false;
	}
	if (s->array_content == NULL) {
		return false;
	}
	//维度数为零时记录第一个维度
	if (s->array_content->dimensions == 0 && s->array_content->size == NULL) {
		s->array_content->dimensions++;
		int *temp = (int *) malloc(sizeof(int) * s->array_content->dimensions);
		temp[s->array_content->dimensions - 1] = size;
		s->array_content->size = temp;
		return true;
	}
	//维度数非零时将新增维度记录在原有size前，形成一个倒序的size记录
	else if (s->array_content->dimensions > 0 && s->array_content->size != NULL) {
		s->array_content->dimensions++;
		int *temp = (int *) malloc(sizeof(int) * s->array_content->dimensions);
		temp[0] = size;
		memcpy(&temp[1], s->array_content->size, s->array_content->dimensions * sizeof(int) - sizeof(int));
		free(s->array_content->size);
		s->array_content->size = temp;
		return true;
	}
	else {
		return false;
	}
}

/**
 * 设置数组类型
 * @param s 符号表指针
 * @param type 要赋给符号表的数组类型
 * @param name 数组名字
 * @return 返回布尔值表示是否成功
 */
bool setArrayType(Symbol *s, ValueTypes type, char *name) {
	//前提：表项存在，数组内容非默认
	if (s == NULL) {
		return false;
	}
	if (s->array_content == NULL) {
		return false;
	}
	//考虑是不是结构数组，两种情况
	if (type != _STRUCT_TYPE_) {
		s->array_content->type = type;
	}
	else {
		s->array_content->type = type;
		//去符号表查找该结构名
		Symbol *struct_def = get(symbolTable, name);
		if (struct_def == NULL || struct_def->symbol_type != STRUCT_TYPE_SYMBOL) {
			return false;
		}
		s->array_content->typeName = (char *) malloc(sizeof(char) * strlen(name) + 1);
		strcpy(s->array_content->typeName, name);
	}
	return true;
}

/**
 * 设置函数返回值
 * @param s 符号表指针
 * @param type 函数返回值的类型
 * @param name 返回值的名字
 * @return 返回布尔值表示是否成功
 */
bool setFuncReturnValue(Symbol *s, ValueTypes type, char *name) {
	if (s == NULL) {
		return false;
	}
	if (s->func_content == NULL) {
		return false;
	}
	if (type != _STRUCT_TYPE_) {
		s->func_content->retType = type;
	}
	else {
		s->func_content->retType = type;
		Symbol *struct_def = get(symbolTable, name);
		if (struct_def == NULL || struct_def->symbol_type != STRUCT_TYPE_SYMBOL) {
			//printf("When setting func return value, this type doesn't exist.\n");
			return false;
		}
		s->func_content->typeName = (char *) malloc(sizeof(char) * strlen(name) + 1);
		strcpy(s->func_content->typeName, name);
	}
	return true;
}

/**
 * 增加函数形参，形参是一个一个串起来的
 * @param s 符号表指针
 * @param name 形参名字
 * @return 返回布尔值表示操作是否成功
 */
bool addFuncArgument(Symbol *s, char *name) {
	if (s == NULL) {
		return false;
	}
	if (s->func_content == NULL) {
		return false;
	}
	if (!isContain(symbolTable, name)) {
		return false;
	}
	if (s->func_content->arguments == NULL) {
		s->func_content->arguments = (Argument *) malloc(sizeof(Argument));
		s->func_content->arguments->name = (char *) malloc(sizeof(char) * strlen(name) + 1);
		strcpy(s->func_content->arguments->name, name);
		s->func_content->arguments->next = NULL;
	}
	else {
		Argument *p = s->func_content->arguments;
		while (p->next != NULL) {
			p = p->next;
		}
		Argument *temp = (Argument *) malloc(sizeof(Argument));
		temp->name = (char *) malloc(sizeof(char) * strlen(name) + 1);
		strcpy(temp->name, name);
		temp->next = NULL;
		p->next = temp;
	}
	return true;
}

/**
 * 设置结构类型
 * @param s 符号表指针
 * @param name 结构体名字
 * @return 返回布尔值表示操作是否成功
 */
bool setStructValueType(Symbol *s, char *name) {
	//前提：符号表项存在，结构值部分非默认
	if (s == NULL) {
		return false;
	}
	if (s->struct_value == NULL) {
		return false;
	}
	//在符号表中查找结构名
	Symbol *struct_def = get(symbolTable, name);
	if (struct_def == NULL || struct_def->symbol_type != STRUCT_TYPE_SYMBOL) {
		return false;
	}
	s->struct_value->typeName = (char *) malloc(sizeof(char) * strlen(name) + 1);
	strcpy(s->struct_value->typeName, name);
	return true;
}

/**
 * 增加结构体内的成员，串起来
 * @param s 符号表指针
 * @param name 成员名字
 * @return 返回布尔值表示操作是否成功
 */
bool addStructTypeField(Symbol *s, char *name) {
	//前提：符号表项存在，结构值部分非默认
	if (s == NULL) {
		return false;
	}
	if (s->struct_value == NULL) {
		return false;
	}
	//在符号表查找这个结构体名字，不存在则报错
	if (!isContain(symbolTable, name)) {
		//printf("When adding struct type field, this field doesn't exist.\n");
		return false;
	}
	//第一个东西特殊处理：申请空间，存名字，初始化指针
	if (s->struct_def->fields == NULL) {
		s->struct_def->fields = (Field *) malloc(sizeof(Field));
		s->struct_def->fields->name = (char *) malloc(sizeof(char) * strlen(name) + 1);
		strcpy(s->struct_def->fields->name, name);
		s->struct_def->fields->next = NULL;
	}
		//之后的处理方法：申请空间、赋值，与前面相连接
	else {
		Field *p = s->struct_def->fields;
		while (p->next != NULL) {
			p = p->next;
		}
		Field *temp = (Field *) malloc(sizeof(Field));
		temp->name = (char *) malloc(sizeof(char) * strlen(name) + 1);
		strcpy(temp->name, name);
		temp->next = NULL;
		p->next = temp;
	}
	return true;
}

/**
 * 返回字符串形式的值类型，方便打印
 * @param type 输入符号类型
 * @return 返回字符串形式的符号类型
 */
char *valueTypesToString(ValueTypes type) {
	switch (type) {
		case _INT_TYPE_:
			return "INT";
		case _FLOAT_TYPE_:
			return "FLOAT";
		case _ARRAY_TYPE_:
			return "ARRAY";
		case _STRUCT_TYPE_:
			return "STRUCT";
		default:
			return "NONE";
	}
}

/**
 * 输出符号表
 * @param s 符号表指针
 * @return 返回布尔值表示操作是否成功
 */
bool outputSymbol(Symbol *s, FILE *file) {
	fprintf(file, "----------------------------------\n");
	if (s == NULL) {
		fprintf(file, "When outputting symbol, this symbol pointer is NULL.\n");
		return false;
	}
	if (s->name != NULL) {
		fprintf(file, "Symbol name: %s\n", s->name);
	}
	else {
		fprintf(file, "Symbol name is NULL\n");
		return false;
	}

	if (s->symbol_type == INT_SYMBOL) {
		fprintf(file, "Symbol type: INT\n");
	}
	else if (s->symbol_type == FLOAT_SYMBOL) {
		fprintf(file, "Symbol type: FLOAT\n");
	}
	else if (s->symbol_type == FUNC_SYMBOL) {
		fprintf(file, "Symbol type: FUNC\n");
		if (s->func_content->retType == _STRUCT_TYPE_) {
			fprintf(file, "Return Type: %s %s\n", valueTypesToString(s->func_content->retType), s->func_content->typeName);
		}
		else {
			fprintf(file, "Return Type: %s\n", valueTypesToString(s->func_content->retType));
		}
		fprintf(file, "Arguments: \n");
		Argument *a = s->func_content->arguments;
		while (a != NULL) {
			fprintf(file, "\t%s\n", a->name);
			a = a->next;
		}
	}
	else if (s->symbol_type == ARRAY_SYMBOL) {
		fprintf(file, "Symbol type: ARRAY\n");
		if (s->array_content->type == _STRUCT_TYPE_) {
			fprintf(file, "Element Type: %s %s\n", valueTypesToString(s->array_content->type), s->array_content->typeName);
		}
		else {
			fprintf(file, "Element Type: %s\n", valueTypesToString(s->array_content->type));
		}
		fprintf(file, "Array dimensions: \n");
		for (int i = 0; i < s->array_content->dimensions; i++) {
			fprintf(file, "%d ", s->array_content->size[i]);
		}
		fprintf(file, "\n");
	}
	else if (s->symbol_type == STRUCT_TYPE_SYMBOL) {
		fprintf(file, "Symbol type: STRUCT_TYPE\n");
		fprintf(file, "Struct fields: \n");
		Field *f = s->struct_def->fields;
		while (f != NULL) {
			fprintf(file, "\t%s\n", f->name);
			f = f->next;
		}
	}
	else if (s->symbol_type == STRUCT_VAL_SYMBOL) {
		fprintf(file, "Symbol type: STRUCT_VAL\n");
		fprintf(file, "Struct type: %s\n", s->struct_value->typeName);
	}
	else {
		fprintf(file, "wrong type.\n");
		return false;
	}
	fprintf(file, "----------------------------------\n");

	return true;
}

/**
 * 比较两个表达式类型是否相同 <br/>
 * 对于数组类型，比较基类型和维数 <br/>
 * 对于 struct， 比较内容定义是否等价
 * @param t1 第一个表达式
 * @param t2 第二个表达式
 * @return 返回是否相同
 */
bool expTpyeEqual(ExpType *t1, ExpType *t2) {
	if (t1->type != t2->type) {
		return false;
	}
	if (t1->type == _ARRAY_TYPE_) {
		return arrayTypeEqual(t1->arrayContent, t2->arrayContent, false);
	}
	if (t1->type == _STRUCT_TYPE_) {
		if (strcmp(t1->typeName, t2->typeName) == 0) {
			return true;
		}
		else {
			return structTypeEqual(get(symbolTable, t1->typeName)->struct_def,
			                       get(symbolTable, t2->typeName)->struct_def);
		}
	}
	return true;
}

/**
 * 比较两个结构体类型是否相同（结构等价）
 * @param s1 第一个结构体
 * @param s2 第二个结构体
 * @return 返回是否相同
 */
bool structTypeEqual(StructTypeContent *s1, StructTypeContent *s2) {
	Field *f1 = s1->fields;
	Field *f2 = s2->fields;
	while (f1 != NULL) {
		if (f2 == NULL) {
			return false;
		}
		Symbol *f1s = get(symbolTable, f1->name);
		Symbol *f2s = get(symbolTable, f2->name);
		if (f1s->symbol_type != f2s->symbol_type) {
			return false;
		}
		else if (f1s->symbol_type == ARRAY_SYMBOL) {
			if (!arrayTypeEqual(f1s->array_content, f2s->array_content, false)) {
				return false;
			}
		}
		else if (f1s->symbol_type == STRUCT_VAL_SYMBOL) {
			if (!structTypeEqual(get(symbolTable, f1s->struct_value->typeName)->struct_def,
			                     get(symbolTable, f2s->struct_value->typeName)->struct_def)) {
				return false;
			}
		}
		f1 = f1->next;
		f2 = f2->next;
	}

	if (f2 != NULL) {
		return false;
	}
	return true;
}

/**
 * 比较两个数组是否同类型， useLength控制是否考虑每一维的长度
 * @param a1 第一个数组
 * @param a2 第二个数组
 * @param useLength 是否比较每一维的大小
 * @return 是否相同
 */
 bool arrayTypeEqual(ArrayContent *a1, ArrayContent *a2, bool useLength) {
	if (a1->type != a2->type) {
		return false;
	}
	else if (a1->type == _STRUCT_TYPE_) {
		if (!structTypeEqual(get(symbolTable, a1->typeName)->struct_def, get(symbolTable, a2->typeName)->struct_def)) {
			return false;
		}
	}
	if (!useLength) {
		return a1->dimensions == a2->dimensions;
	}
	else {
		if (a1->dimensions != a2->dimensions) {
			return false;
		}
		else {
			//TODO 这里可能有点隐患 为啥？ 我感觉可能越界哈！
			for (int i = 0; i < a1->dimensions; i++) {
				if (a1->size[i] != a2->size[i]) {
					return false;
				}
			}
			return true;
		}
	}
}

/**
 * filedName 这个字段是否是 struct 类型 s 的 field
 * @param s 结构体
 * @param fieldName 作用域名字
 * @return 是否是struct的field
 */
bool isField(StructTypeContent *s, char *fieldName) {
	Field *f = s->fields;
	while (f != NULL) {
		if (strcmp(f->name, fieldName) == 0) {
			return true;
		}
		f = f->next;
	}
	return false;
}

/**
 * 基本类型 int float</br>
 * 结构体类型调用structTypeEqual
 * 看形参和实参是否匹配
 * @param args 形参
 * @param parameters 实参
 * @return 是否匹配
 */
bool argsMatch(Argument *args, ParaType *parameters) {
	if (args == NULL && parameters == NULL) {
		return true;
	}
	else if ((args == NULL && parameters != NULL) || (args != NULL && parameters == NULL)) {
		return false;
	}
	else {
		Symbol *s1 = get(symbolTable, args->name);
		switch (s1->symbol_type) {
			case INT_SYMBOL:
				if (parameters->type != _INT_TYPE_) {
					return false;
				}
				else {
					return argsMatch(args->next, parameters->next);
				}
			case FLOAT_SYMBOL:
				if (parameters->type != _FLOAT_TYPE_) {
					return false;
				}
				else {
					return argsMatch(args->next, parameters->next);
				}
			case ARRAY_SYMBOL:
				if (parameters->type != _ARRAY_TYPE_ ||
				    (!arrayTypeEqual(s1->array_content, parameters->arrayContent, true))) {
					return false;
				}
				else {
					return argsMatch(args->next, parameters->next);
				}
			case STRUCT_VAL_SYMBOL:
				if (parameters->type != _STRUCT_TYPE_) {
					return false;
				}
				else if (strcmp(parameters->typeName, s1->struct_value->typeName) == 0) {
					return true;
				}
				else {
					StructTypeContent *struct1 = get(symbolTable, s1->struct_value->typeName)->struct_def;
					StructTypeContent *struct2 = get(symbolTable, parameters->typeName)->struct_def;
					if (!structTypeEqual(struct1, struct2)) {
						return false;
					}
					else {
						return argsMatch(args->next, parameters->next);
					}
				}
			default:
				break;
		}
	}
}


