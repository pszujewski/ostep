#include "./list.h"

#include <stdio.h>
#include <stdlib.h>

// gcc ./test-list.c ./list.c -o ./test-list -Wall -Werror -g3

int main()
{
    list_node *list = list_init();
    char *peter = (char *)malloc(sizeof(char) * 6);
    *(peter) = 'p';
    *(peter + 1) = 'e';
    *(peter + 2) = 't';
    *(peter + 3) = '\0';
    char *chelsea = (char *)malloc(sizeof(char) * 7);
    *(chelsea) = 'c';
    *(chelsea + 1) = 'h';
    *(chelsea + 2) = 'e';
    *(chelsea + 3) = '\0';
    size_t peterIdx = list_insert(list, peter);
    size_t chelseaIdx = list_insert(list, chelsea);
    list_node *node = get_node(list, 1);
    char *found_name = (char *)(node->data);
    printf("Found name should be Chelsea: %s\n", found_name);
    printf("peterIdx should be 0: %ld\n", peterIdx);
    printf("chelseaIdx should be 1: %ld\n", chelseaIdx);
    list_free(list);
}