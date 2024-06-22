#include "tree.h"

extern Node* root;

const char *const TYPES_NAME_TABLE[] = {
		/* 0 Blank */
		"BLANK",
		/* 1 Tokens */
		"SEMI", "COMMA", "ASSIGNOP", "RELOP",
		"PLUS", "MINUS", "STAR", "DIV",
		"AND", "OR", "DOT", "NOT", "TYPE",
		"LP", "RP", "LB", "RB", "LC", "RC",
		"STRUCT", "RETURN", "IF", "ELSE", "WHILE",
		"ID", "INT", "FLOAT",
		/* 2 High-level Definitions */
		"Program", "ExtDefList", "ExtDef", "ExtDecList",
		/* 3 Specifiers */
		"Specifier", "StructSpecifier", "OptTag", "Tag",
		/* 4 Declarators */
		"VarDec", "FunDec", "VarList", "ParamDec",
		/* 5 Statements */
		"CompSt", "StmtList", "Stmt",
		/* 6 Local Definitions */
		"DefList", "Def", "DecList", "Dec",
		/* 7 Expressions */
		"Exp", "Args"
};

/**
 * 将节点 Node 类型转换为字符串
 * @param type 节点类型
 * @return 字符串的首指针
 */
const char *typeToString(Types type) {
	return TYPES_NAME_TABLE[type];
}

/**
 * 新建立一个节点
 * @param type 节点类型
 * @param lineNum 节点所在的行数
 * @return 新结点指针
 */
Node *createNode(Types type, int lineNum) {
	// 申请结点内存
	Node *node = (Node *) malloc(sizeof(Node));
	if (node == NULL) {
		fprintf(stderr, "Memory alloc error in createNode()\n");
		exit(0);
	}

	// 初始化结点内容
	node->child = node->brother = NULL;
	node->type = type;
	node->lineNumber = lineNum;

	return node;
}

/**
 * 为一个子节点插入父节点
 * @param father 爸爸节点
 * @param child 孩子节点
 */
void addChild(Node *father, Node *child) {
	// 爹没孩子就给个孩子，爹有孩子，就去当孩子的兄弟
	if (father->child == NULL) father->child = child;
	else {
		Node *chd = father->child;
		while (chd->brother != NULL) chd = chd->brother;
		chd->brother = child;
	}
}

/**
 * 为多个子节点插入父节点
 * @param type 父节点类型
 * @param lineNum 父节点所在的行数
 * @param n 子节点个数
 * @param ... 子节点参数包
 * @return 父节点指针
 */
Node *insertNode(Types type, int lineNum, int n, ...) {
	// 申请内存
	Node *father = (Node *) malloc(sizeof(Node));
	if (father == NULL) {
		fprintf(stderr, "Memory alloc error in insertNode()\n");
		exit(0);
	}

	// 初始化爸爸节点
	father->brother = father->child = NULL;
	father->type = type;
	father->lineNumber = lineNum;


	// 将参数包中的所有子节点都和父节点产生关联
	va_list vaList;
	va_start(vaList, n);

	if (n == 0) {
		addChild(father, createNode(_BLANK, 0));
	}
	else {
		for (int i = 0; i < n; ++i) {
			addChild(father, va_arg(vaList, Node *));
		}
	}
	va_end(vaList);

	return father;
}

/**
 * 打印单个节点
 * @param node 需要打印的结点指针
 */
void printNode(Node *node, FILE *f) {
	if (node == NULL || node->type == _BLANK) return;
	if (root->type >= 28) {
		fprintf(f, "%s (%d)\n", typeToString(node->type), node->lineNumber);
	}
	else if (root->type == _INT) {
		fprintf(f, "%s: %d\n", typeToString(node->type), root->intValue);
	}
	else if (root->type == _FLOAT) {
		fprintf(f, "%s: %f\n", typeToString(node->type), root->floatValue);
	}
	else {
		fprintf(f, "%s: %s\n", typeToString(node->type), root->idName);
	}
}

/**
 * 打印语法树
 * @param node 语法树的根结点
 * @param depth 遍历语法树所到的深度
 */
void printTree(Node *node, int depth, FILE *f) {
	if (node == NULL) return;
	for (int i = 0; i < depth; ++i) fprintf(f, "\t");
	printNode(node, f);
	printTree(node->child, depth + 1, f);
	printTree(node->brother, depth, f);
}

/**
 * 销毁语法树
 * @param node 语法树的根结点
 */
void destroyTree(Node *node) {
	if (node == NULL) return;
	if (node->type < 28 && node->type != _INT && node->type != _FLOAT)
		free(node->idName);
	Node *c = node->child;
	while (c != NULL) {
		destroyTree(c);
		c = c->brother;
	}
	free(node);
}