#ifndef __HASH_SET__
#define __HASH_SET__

#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "symbol.h"

#define HASH_SIZE 16384

typedef struct Bucket {
	Symbol *head;
} Bucket;

typedef struct HashTable {
	int size;
	Bucket *buckets;
} HashTable;

HashTable *initializeHashSet(int size);

unsigned int pjwHash(char *str);

bool isContain(HashTable *hashTable, char *name);

bool insert(HashTable *hashTable, Symbol *symbol);

Symbol *get(HashTable *hashTable, char *name);

void outputHashTable(HashTable *hashTable, FILE *f);

#endif
