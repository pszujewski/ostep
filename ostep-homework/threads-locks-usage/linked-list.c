#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include "common_threads.h"

#define NUMTHREADS 12

typedef struct __list_node_t
{
    int index;
    struct __list_node_t *next;
    void *data;
} ListNode;

typedef struct __list_update_args_t
{
    int index;
    int incrementor;
    struct __list_node_t *list;
} ListUpdateArgs;

void List_Init(ListNode *list);
int List_Insert(ListNode *list, void *data);

void List_Free(ListNode *list);
ListNode *List_Get(ListNode *node, int index);

void Test_Insert(ListNode *list, int data);
void List_Update(ListNode *list, int index, int inc);

void *Test_Update(void *args);
int Test_List_Get_Value(ListNode *list, int index);

pthread_mutex_t list_lock;

int main()
{
    pthread_mutex_init(&list_lock, NULL);

    ListNode *root = (ListNode *)malloc(sizeof(ListNode));
    List_Init(root);

    Test_Insert(root, 99);
    Test_Insert(root, 78);
    Test_Insert(root, 42);
    Test_Insert(root, 22);
    Test_Insert(root, 50);

    for (int i = 0; i < 50; i++)
    {
        Test_Insert(root, 100);
    }

    int *data = (int *)(List_Get(root, 3)->data);
    assert(*data == 22);

    pthread_t pid[NUMTHREADS];

    for (size_t i = 0; i < NUMTHREADS; i++)
    {
        ListUpdateArgs *args = malloc(sizeof(ListUpdateArgs));

        args->list = root;
        args->index = 10;
        args->incrementor = 2;

        Pthread_create(&pid[i], NULL, Test_Update, (void *)args);
    }

    for (size_t i = 0; i < NUMTHREADS; i++)
    {
        Pthread_join(pid[i], NULL);
    }

    int value_at_index = Test_List_Get_Value(root, 10);
    printf("Value at index 10 is %d\n", value_at_index);

    assert(value_at_index == 100 + (NUMTHREADS * 2));
    List_Free(root);
}

int Test_List_Get_Value(ListNode *list, int index)
{
    return *(int *)List_Get(list, index)->data;
}

void Test_Insert(ListNode *list, int data)
{
    int *addr = (int *)malloc(sizeof(int)); // int *addr is an address (pointer) to a "int" value in memory. The "*" is technically part of the variable name.
    *addr = data;                           // "*" here is an operator saying "addr" is a variable of type "int pointer". Access the value at that pointer. So "addr" is a pointer var and "*addr" is an int.

    int idx = List_Insert(list, (void *)addr);
    int *foundaddr = (int *)(List_Get(list, idx)->data);

    assert(*foundaddr == data);
}

void *Test_Update(void *args)
{
    ListUpdateArgs *uargs = (ListUpdateArgs *)args;
    List_Update(uargs->list, uargs->index, uargs->incrementor);
    return NULL;
}

void List_Update(ListNode *list, int index, int inc)
{
    pthread_mutex_lock(&list_lock);

    ListNode *item = List_Get(list, index);
    int data = *(int *)item->data; // convert void * to int pointer and then get the data pointed to (the int)

    int *new_data = malloc(sizeof(int));
    *new_data = data + inc; // new_data is a pointer. Use "*" operator to access the data at the pointer.

    item->data = (void *)new_data;
    item = List_Get(list, index);

    assert(*(int *)item->data == *new_data);
    pthread_mutex_unlock(&list_lock);
}

void List_Init(ListNode *list)
{
    list->index = 0;
    list->next = NULL;
    list->data = NULL;
}

int List_Insert(ListNode *list, void *data_address)
{
    int index = 0;

    if (list->data == NULL)
    {
        list->data = data_address;
    }
    else
    {
        ListNode *curr = list;
        index++;

        while (curr->next != NULL)
        {
            curr = curr->next;
            index++;
        }

        ListNode *node = (ListNode *)malloc(sizeof(ListNode));
        List_Init(node);

        node->data = data_address;
        node->index = index;

        curr->next = node;
    }
    return index;
}

ListNode *List_Get(ListNode *node, int index)
{
    if (node == NULL)
    {
        return NULL;
    }

    if (node->index == index)
    {
        return node;
    }

    return List_Get(node->next, index);
}

void List_Free(ListNode *list)
{
    if (list == NULL)
    {
        return;
    }

    ListNode *next = list->next;

    if (list->data != NULL)
    {
        free(list->data);
    }

    free(list);

    List_Free(next);
}
