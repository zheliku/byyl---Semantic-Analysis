#include "hashTable.h"

HashTable *symbolTable = NULL;

HashTable *initializeHashSet(int size) {
	HashTable *pHashTable = (HashTable *) malloc(sizeof(HashTable));
	if (pHashTable == NULL) {
		return NULL;
	}
	pHashTable->size = size;
	pHashTable->buckets = (Bucket *) malloc(sizeof(Bucket) * size);
	for (int i = 0; i < size; i++) {
		pHashTable->buckets[i].head = NULL;
	}

	return pHashTable;
}

unsigned int pjwHash(char *str) {
	str = (unsigned char *) str;
	unsigned int val = 0;
	unsigned int i;
	for (; *str; ++str) {
		val = (val << 2) + *str;
		if ((i = val & ~0x3fff))
			val = (val ^ (i >> 12)) & 0x3fff;
	}

	return val;
}

bool isContain(HashTable *hashTable, char *name) {
	if (hashTable == NULL) {
		printf("\033[31mthe hash set is NULL\n\033[0m");
		return false;
	}
	if (name == NULL) {
		printf("\033[31mthe name is NULL\n\033[0m");
		return false;
	}

	unsigned int val = pjwHash(name) % HASH_SIZE;
	Symbol *head = hashTable->buckets[val].head;
	while (head != NULL) {
		if (strcmp(name, head->name) == 0) {
			return true;
		}
		head = head->next;
	}

	return false;
}

Symbol *get(HashTable *hashTable, char *name) {
	if (name == NULL) {
		printf("\033[31mthe name is NULL\n\033[0m");
		return NULL;
	}
	if (hashTable == NULL) {
		printf("\033[31mthe hash set is NULL\n\033[0m");
		return NULL;
	}

	unsigned int val = pjwHash(name) % HASH_SIZE;
	Symbol *head = hashTable->buckets[val].head;
	while (head != NULL) {
		if (strcmp(name, head->name) == 0) {
			return head;
		}
		head = head->next;
	}

	return NULL;
}

bool insert(HashTable *hashTable, Symbol *symbol) {
	if (symbol->name == NULL) {
		printf("\033[31mthe name is NULL\n\033[0m");
		return false;
	}
	if (isContain(hashTable, symbol->name)) {
		return false;
	}
	else {
		unsigned int val = pjwHash(symbol->name) % HASH_SIZE;
		symbol->next = hashTable->buckets[val].head;
		hashTable->buckets[val].head = symbol;
		return true;
	}
}

void outputHashTable(HashTable *hashTable, FILE *f) {
	for (int i = 0; i < hashTable->size; i++) {
		Symbol *p = hashTable->buckets[i].head;
		while (p != NULL) {
			outputSymbol(p, f);
			fprintf(f, "\n");
			p = p->next;
		}
	}
}
