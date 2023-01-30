#include "../list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// gcc ./test-list.c ../list.c -o ./test-list -Wall -Werror -g

char *copyString(char *s)
{
    char *s2;
    s2 = (char *)malloc(sizeof(s));

    strcpy(s2, s);
    return (char *)s2;
}

int *getInt(int n)
{
    int *p = (int *)malloc(sizeof(n));
    *p = n;
    return p;
}

void int_test()
{
    list_node *list = list_init();

    list_insert(list, getInt(100));
    list_insert(list, getInt(101));
    list_insert(list, getInt(102));
    list_insert(list, getInt(103));
    list_insert(list, getInt(104));
    list_insert(list, getInt(105));
    list_insert(list, getInt(106));
    list_insert(list, getInt(107));

    list_node *node = list_get(list, 6);
    int *found = (int *)(node->data);

    printf("Found int 106: %d\n", *found);
    list_free(list);
}

void string_test()
{
    list_node *list = list_init();

    char *peter = "Peter";
    char *chelsea = "Chelsea";
    char *stella = "Stella";

    size_t peterIdx = list_insert(list, copyString(peter));
    size_t chelseaIdx = list_insert(list, copyString(chelsea));
    list_insert(list, copyString(stella));

    list_node *node = list_get(list, 1);
    char *found_name = (char *)(node->data);

    list_node *second = list_get(list, 2);
    char *otherFound = (char *)(second->data);

    printf("Found name should be Chelsea: %s\n", found_name);
    printf("Other Found name should be Stella: %s\n", otherFound);

    printf("peterIdx should be 0: %ld\n", peterIdx);
    printf("chelseaIdx should be 1: %ld\n", chelseaIdx);

    list_free(list);
}

int main()
{
    int_test();
    string_test();
}