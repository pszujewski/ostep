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
    // char *path[] = {"/bin", NULL};

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
            printf("Exiting...\n");
            exit(1);
        }

        parseAndRunCommands(input, 0);

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

void parseAndRunCommands(char *input, size_t start)
{
    int length = strlen(input);
    list_node *inputTokens = list_init();

    char *token = (char *)malloc(50);
    token[0] = '\0';

    for (size_t i = start; i < length; i++)
    {
        if (input[i] == '&')
        {
            runCommand(inputTokens);
            list_free(inputTokens);
            size_t nextCmdStartIdx = i + 2;
            return parseAndRunCommands(input, nextCmdStartIdx);
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
    runCommand(inputTokens);
    list_free(inputTokens);
}

void runCommand(list_node *inputTokens)
{
    char *args[25];
    size_t size = list_get_size(inputTokens);

    char *cmd = getCommand(inputTokens);
    setArgumentsList(args, inputTokens, size);
    // isExecutable?
    exec(cmd, args);
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
    char *cmd = strcat("/", command);

    int index = 0;
    while (path[index] != NULL)
    {
        int exists = access(strcat(path[index], cmd), X_OK);
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

void error()
{
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}