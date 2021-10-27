#ifndef _WISH_H
#define _WISH_H

#include <stdbool.h>
#include "../lib/list.h"

void error();

void interactive(char *input);

char *getCommand(list_node *inputTokens);

void parseAndRunCommands(char *input, size_t start);

void runCommand(list_node *inputTokens);

void setArgumentsList(char *args[], list_node *inputTokens, size_t size);

void exec(char *command, char *args[]);

bool isExecutable(char *command, char *path[]);

#endif // _WISH_H