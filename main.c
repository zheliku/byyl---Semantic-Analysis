#include <stdio.h>
#include "tree.h"
#include "semantic.h"
#include "symbol.h"
#include "hashTable.h"
#include "log.h"

extern void yyrestart(FILE *);

extern void yyparse(void);

extern Node *root;
extern int syntax_correct;
extern int lexical_correct;
extern HashTable *symbolTable;

int main(int argc, char **argv) {
	if (argc <= 1) {
		printf("pass filename to scanner\n");
		return -1;
	}
	else {
		FILE *f = fopen(argv[1], "r");
		if (!f) {
			printf("fopen failed:%s\n", argv[1]);
			return -1;
		}
		yyrestart(f);
		yyparse();
		FILE *semanticAnalysisLogFile = fopen("SemanticAnalysisLog.txt", "w");
		FILE *grammarTreeFile = fopen("grammarTree.txt", "w");
		FILE *hashTableFile = fopen("hashTable.txt", "w");
		if (syntax_correct && lexical_correct) {
			symbolTable = initializeHashSet(HASH_SIZE);
			SemanticAnalysisLog = initLog();
			SemanticError = initLog();
			printTree(root, 0, grammarTreeFile);
			dealWithProgram(root);
            checkFunction();
			outputLog(SemanticAnalysisLog, semanticAnalysisLogFile);
			outputHashTable(symbolTable, hashTableFile);
			outputLog(SemanticError, stdout);
//            printFunctions();
		}
		destroyTree(root);
		return 0;
	}
}
