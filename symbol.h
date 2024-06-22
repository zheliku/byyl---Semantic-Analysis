#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

// 符号有五种类型：int、float、array、struct、function
typedef enum SymbolTypes {
	INT_SYMBOL = 1,
	FLOAT_SYMBOL = 2,
	ARRAY_SYMBOL = 3,
	STRUCT_TYPE_SYMBOL = 4, // 这是定义结构体时加入符号表的
	STRUCT_VAL_SYMBOL = 5,  // 这是定义结构体实例时加入符号表的
	FUNC_SYMBOL = 6
} SymbolTypes;

// 值有四种类型：int、float、struct、array
// 从 1 开始是为了把 0 作为未定义的标志
typedef enum ValueTypes {
	_INT_TYPE_ = 1,
	_FLOAT_TYPE_ = 2,
	_STRUCT_TYPE_ = 3,
	_ARRAY_TYPE_ = 4,
} ValueTypes;

// 以下是各个符号的内容
// int 和 float 要存储变量的值
// INT_SYMBOL
typedef struct IntContent {
	int val;
} IntContent;

//FLOAT_SYMBOL
typedef struct FloatContent {
	float val;
} FloatContent;

// ARRAY_SYMBOL
// 数组要存储数组元素的类型、数组维数、每个维度的大小。
// 特别地，结构数组要存储结构的名字，因此定义 typename
typedef struct ArrayContent {
	ValueTypes type;
	int dimensions;
	int *size;
	char *typeName;
} ArrayContent;

// 形参列表的内容，作为函数符号的一个部分
// 记录每个形参名，以链表形式组织
typedef struct Argument {
	char *name;
	struct Argument *next;
} Argument;

// FUNC_SYMBOL
// 包含形参表和返回值，特别地，返回结构体时要记录结构名
typedef struct FuncContent {
	ValueTypes retType;
	char *typeName;
	Argument *arguments;
} FuncContent;

// STRUCT_DEF_SYMBOL
// 要记录结构体中各个部分的名字，用链表组织
typedef struct Field {
	char *name;
	struct Field *next;
} Field;

// 这是链表起始指针，跟上面合起来作为结构体完整的符号项
typedef struct StructTypeContent {
	Field *fields;
} StructTypeContent;

// STRUCT_VAL_SYMBOL
// 结构体实例要指明是哪个结构体
typedef struct StructValueContent {
	char *typeName;
} StructValueContent;

// Expression 可以是 int、float、array、struct
// 有些表达式不能做左值，为了判断该错误加入布尔变量
typedef struct ExpType {
	bool leftValue;
	ValueTypes type;
	/* 额外字段 */
	union {
		//如果type是
		char *typeName;
		struct ArrayContent *arrayContent;
	};
} ExpType;

// 实参的类型
// 不必判断左值的问题，以链表形式组织
typedef struct ParaType {
	ValueTypes type;
	union {
		char *typeName;
		ArrayContent *arrayContent;
	};
	struct ParaType *next;
} ParaType;

// 以链表形式组织符号表
// 下面是单个结点的内容，包括变量名、类型和针对各种类型的内容
typedef struct Symbol {
	char *name;
	SymbolTypes symbol_type;
	union {
		IntContent *int_content;
		FloatContent *float_content;
		FuncContent *func_content;
		ArrayContent *array_content;
		StructTypeContent *struct_def;
		StructValueContent *struct_value;
	};
	struct Symbol *next;
} Symbol;

typedef struct FUNCTION{
    bool dingyi;
    char* name;//函数名
    int line_num;//用于错误打印
    struct FUNCTION* next;
}functionSm;


// 我们将需要对符号表进行的操作封装成以下函数
// 创建符号表的一个项
Symbol *createSymbol();
// 设置符号的名字
bool setSymbolName(Symbol *s, char *name);
// 设置符号的类型
bool setSymbolType(Symbol *s, SymbolTypes type);
// 数组的维度 +1，数组的维度数不能一次获取到，因此需要一个一个累加
bool addArrayDimension(Symbol *s, int size);
// 设置数组项类型
bool setArrayType(Symbol *s, ValueTypes type, char *name);
// 设置函数返回值名字和类型
bool setFuncReturnValue(Symbol *s, ValueTypes type, char *name);
// 增加函数形参，形参是一个一个串起来的
bool addFuncArgument(Symbol *s, char *name);
// 设置结构体类型
bool setStructValueType(Symbol *s, char *name);
// 增加结构体内的东西，这些东西也是串起来的
bool addStructTypeField(Symbol *s, char *name);
// 这个函数方便我们获得字符串形式的类型
char *valueTypesToString(ValueTypes type);
// 输出符号表
bool outputSymbol(Symbol *s, FILE *f);
// 比较两个表达式类型是否相同
bool expTpyeEqual(ExpType *t1, ExpType *t2);
// 比较两个结构体类型是否相同（结构等价）
bool structTypeEqual(StructTypeContent *s1, StructTypeContent *s2);
// 比较两个数组是否同类型， useLength控制是否考虑每一维的长度
bool arrayTypeEqual(ArrayContent *a1, ArrayContent *a2, bool useLength);
// filedName 这个字段是否是 struct 类型 s 的 field
bool isField(StructTypeContent *s, char *fieldName);
// 类型检查：基本类型 int、float
// 结构体类型调用 structTypeEqual
bool argsMatch(Argument *args, ParaType *parameters);

#endif
