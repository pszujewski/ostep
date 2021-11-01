#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <ctype.h>
#include "./wish.h"
#include "../lib/list.h"

#define MAX_INPUT_SZ 256

int main(int argc, char *argv[])
{
    char *path[100];
    initPath(path);

    if (argc > 1)
    {
        batch(argc, argv, path);
        freePath(path);
        exit(0);
    }

    while (argc == 1)
    {
        char *input = malloc(MAX_INPUT_SZ);

        if (input == NULL)
        {
            freePath(path);
            error();
        }

        // int index = 0;

        // while (path[index] != NULL)
        // {
        //     printf("%s\n", path[index]);
        //     index++;
        // }

        // index = 0;

        interactive(input);
        evalForExit(input, path);

        parseAndRunCommands(input, 0, path);
        free(input);
    }

    freePath(path);
    exit(1);
}

void evalForExit(char *input, char *path[])
{
    if (strcmp(input, "exit") == 0)
    {
        freePath(path);
        exit(0);
    }
}

void batch(int argc, char *argv[], char *path[])
{
    for (int i = 1; i < argc; i++)
    {
        FILE *fp = fopen(argv[i], "r");

        if (fp == NULL)
        {
            error();
        }

        char ipt[50];

        while (fgets(ipt, 50, fp))
        {
            strclean(ipt);
            evalForExit(ipt, path);
            parseAndRunCommands(ipt, 0, path);
        }

        fclose(fp);
    }
}

void interactive(char *input)
{
    printf("wish> ");
    fgets(input, MAX_INPUT_SZ, stdin);
    strclean(input);
}

void strclean(char *input)
{
    if ((strlen(input) > 0) && (input[strlen(input) - 1] == '\n'))
    {
        input[strlen(input) - 1] = '\0';
    }
}

void parseAndRunCommands(char *input, size_t start, char *path[])
{
    int length = strlen(input);
    list_node *inputTokens = list_init();

    char *token = (char *)malloc(50);
    token[0] = '\0';

    for (size_t i = start; i < length; i++)
    {
        if (input[i] == '&')
        {
            runCommand(inputTokens, path);
            list_free(inputTokens);
            size_t nextCmdStartIdx = i + 2;
            return parseAndRunCommands(input, nextCmdStartIdx, path);
        }
        else if (isspace(input[i]) == 0)
        {
            // Not a space
            char temp[2];
            temp[0] = input[i];
            temp[1] = '\0';
            strcat(token, temp);
        }
        else
        {
            // is a space
            if (strlen(token) > 0)
            {
                list_insert(inputTokens, token);
                token = (char *)malloc(50);
                token[0] = '\0';
            }
        }
    }
    if (strlen(token) > 0)
    {
        list_insert(inputTokens, token);
    }

    runCommand(inputTokens, path);
    list_free(inputTokens);
}

void initPath(char *path[])
{
    char *dest = (char *)malloc(25);
    strcpy(dest, "/bin");
    path[0] = dest;
    path[1] = NULL;
}

void freePath(char *path[])
{
    int index = 0;

    while (path[index] != NULL)
    {
        free(path[index]);
        index++;
    }
}

void runCommand(list_node *inputTokens, char *path[])
{
    char *args[25];
    size_t size = list_get_size(inputTokens);

    char *cmd = getCommand(inputTokens);
    setArgumentsList(args, inputTokens, size);

    if (strcmp(cmd, "cd") == 0)
    {
        int result = chdir(args[1]);
        if (result == -1)
        {
            error();
        }
    }
    else if (strcmp(cmd, "path") == 0)
    {
        setNewPath(path, args);
    }
    else
    {
        if (isExecutable(cmd, path))
        {
            exec(cmd, args, path);
        }
        else
        {
            error();
        }
    }
}

char *getCommand(list_node *inputTokens)
{
    list_node *item = list_get(inputTokens, 0);
    return (char *)(item->data);
}

void setArgumentsList(char *args[], list_node *inputTokens, size_t size)
{
    args[0] = getCommand(inputTokens);

    if (size > 1)
    {
        for (size_t i = 1; i < size; i++)
        {
            list_node *item = list_get(inputTokens, i);
            args[i] = (char *)(item->data);
        }
        args[size] = NULL;
    }
    else
    {
        args[1] = NULL;
    }
}

bool isExecutable(char *command, char *path[])
{
    int result = getFullExecutablePathIndex(command, path);
    return result > -1;
}

int getFullExecutablePathIndex(char *command, char *path[])
{
    char cmdfile[20];

    strcpy(cmdfile, "/");
    strcat(cmdfile, command);

    int index = 0;
    while (path[index] != NULL)
    {
        char fullpath[20];

        strcpy(fullpath, path[index]);
        strcat(fullpath, cmdfile);

        int exists = access(fullpath, X_OK);

        if (exists == 0)
        {
            return index;
        }
        index++;
    }
    return -1;
}

// TODO: Must accept "path" variable here
void exec(char *command, char *args[], char *path[])
{
    int rc = fork();

    if (rc < 0)
    {
        // fork failed
        error();
    }
    else if (rc == 0)
    {
        // child path
        int idx = getFullExecutablePathIndex(command, path);

        char cmdfile[20];
        char fullpath[20];

        strcpy(cmdfile, "/");
        strcat(cmdfile, command);

        strcpy(fullpath, path[idx]);
        strcat(fullpath, cmdfile);

        execv(fullpath, args);
        printf("\n");
    }
    else
    {
        // parent path
        wait(NULL);
    }
}

void setNewPath(char *path[], char *args[])
{
    int index = 1;

    while (args[index] != NULL)
    {
        char *dest = (char *)malloc(25);
        strcpy(dest, args[index]);
        path[index - 1] = dest;
        index++;
    }
    path[index - 1] = NULL;
}

void error()
{
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}