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

    while (argc == 1)
    {
        char *input = malloc(MAX_INPUT_SZ);

        if (input == NULL)
        {
            error();
            exit(1);
        }

        interactive(input);

        if (strcmp(input, "exit") == 0)
        {
            exit(0);
        }

        parseAndRunCommands(input, 0, path);

        // char *argument_list[] = {"ls", NULL};
        // exec("ls", argument_list);

        free(input);
    }
    exit(1);
}

void interactive(char *input)
{
    printf("wish> ");
    fgets(input, MAX_INPUT_SZ, stdin);

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
    list_insert(inputTokens, token);
    runCommand(inputTokens, path);
    list_free(inputTokens);
}

void initPath(char *path[])
{
    path[0] = "/bin";
    path[1] = NULL;
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
            exit(1);
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
            exec(cmd, args);
        }
        else
        {
            error();
            exit(1);
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
            return true;
        }
        index++;
    }
    return false;
}

void exec(char *command, char *args[])
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
        execvp(command, args);
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
    if (args[1] == NULL)
    {
        error();
        exit(1);
    }

    int index = 1;

    while (args[index] != NULL)
    {
        path[index - 1] = args[index];
        index++;
    }
    path[index - 1] = NULL;
}

void error()
{
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}