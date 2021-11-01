#ifndef _WISH_H
#define _WISH_H

#include <stdbool.h>
#include "../lib/list.h"

void error();

void batch(int argc, char *argv[], char *path[]);

void freePath(char *path[]);

void interactive(char *input);

void initPath(char *path[]);

void evalForExit(char *input, char *path[]);

void strclean(char *input);

char *getCommand(list_node *inputTokens);

void parseAndRunCommands(char *input, size_t start, char *path[]);

void runCommand(list_node *inputTokens, char *path[]);

void setArgumentsList(char *args[], list_node *inputTokens, size_t size);

void setNewPath(char *path[], char *args[]);

void exec(char *command, char *args[]);

bool isExecutable(char *command, char *path[]);

#endif // _WISH_H