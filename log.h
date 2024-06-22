#ifndef __LOG_H_
#define __LOG_H_

#include <stdbool.h>
#include <stdio.h>
#include "tree.h"

#define MESSAGE_LENGTH 256

typedef struct Info {
	char *content;
	struct Info *next;
} Info;

typedef struct Log {
	Info *head;
	Info *tail;
} Log;

extern Log *SemanticError;
extern Log *SemanticAnalysisLog;

Log *initLog();

bool addLogInfo(Log *log, char *content);

bool outputLog(Log *log, FILE *file);

bool reportError(Log *log, int type, int line, char *message);

bool addLogNullNode(Types type);

bool addLogTypeDismatch(Types type);

bool addLogNullChild(Types type);

bool addLogWrongChild(Types type);

bool addLogStartDealing(Types type);

bool addLogEmptyNode(Types type);

#endif
