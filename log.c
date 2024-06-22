#include "log.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

Log *SemanticError = NULL;
Log *SemanticAnalysisLog = NULL;

Log *initLog() {
	Log *log = (Log *) malloc(sizeof(Log));
	if (log == NULL) return NULL;
	log->head = NULL;
	log->tail = NULL;
	return log;
}

bool addLogInfo(Log *log, char *content) {
	if (log == NULL) {
		printf("The Log is NULL.\n");
		return false;
	}
	else {
		if (log->head == NULL && log->tail == NULL) {
			Info *i = (Info *) malloc(sizeof(Info));
			i->content = (char *) malloc(strlen(content) + 1);
			strcpy(i->content, content);
			i->next = NULL;
			log->head = i;
			log->tail = i;
		}
		else if (log->head != NULL && log->tail != NULL) {
			Info *i = (Info *) malloc(sizeof(Info));
			i->content = (char *) malloc(strlen(content) + 1);
			strcpy(i->content, content);
			i->next = NULL;
			log->tail->next = i;
			log->tail = i;
		}
		else {
			printf("This Log is broken.\n");
			return false;
		}
	}
	return true;
}

bool outputLog(Log *log, FILE *file) {
	if (log == NULL) {
		printf("The Log is NULL.\n");
		return false;
	}
	Info *p = log->head;
	while (p != NULL) {
		fprintf(file, "%s", p->content);
		p = p->next;
	}
	return true;
}

bool reportError(Log *log, int type, int line, char *message) {
	char str[MESSAGE_LENGTH];
	sprintf(str, "Error Type %d at Line %d: %s.\n", type, line, message);
	return addLogInfo(log, str);
}

bool addLogNullNode(Types type) {
	char message[MESSAGE_LENGTH];
	sprintf(message, "when dealing with %s, this %s is NULL.\n", typeToString(type), typeToString(type));
	addLogInfo(SemanticAnalysisLog, message);
	return true;
}

bool addLogTypeDismatch(Types type) {
	char message[MESSAGE_LENGTH];
	sprintf(message, "when dealing with %s, this node is not %s.\n", typeToString(type), typeToString(type));
	addLogInfo(SemanticAnalysisLog, message);
	return true;
}

bool addLogNullChild(Types type) {
	char message[MESSAGE_LENGTH];
	sprintf(message, "when dealing with %s, child node is NULL.\n", typeToString(type));
	addLogInfo(SemanticAnalysisLog, message);
	return true;
}

bool addLogWrongChild(Types type) {
	char message[MESSAGE_LENGTH];
	sprintf(message, "when dealing with %s, this %s has a wrong child.\n", typeToString(type), typeToString(type));
	addLogInfo(SemanticAnalysisLog, message);
	return true;
}

bool addLogStartDealing(Types type) {
	char message[MESSAGE_LENGTH];
	sprintf(message, "Start dealing with %s.\n", typeToString(type));
	addLogInfo(SemanticAnalysisLog, message);
	return true;
}

bool addLogEmptyNode(Types type) {
	char message[MESSAGE_LENGTH];
	sprintf(message, "when dealing with %s, this %s is empty.\n", typeToString(type), typeToString(type));
	addLogInfo(SemanticAnalysisLog, message);
	return true;
}