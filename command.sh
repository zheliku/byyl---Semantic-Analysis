flex lexical.l
bison -d syntax.y
gcc syntax.tab.c main.c -lfl -ly -o parser tree.c symbol.c semantic.c log.c hashTable.c
