#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

typedef struct __list_node_t
{
    int index;
    struct __list_node_t *next;
    void *data;
} ListNode;

void List_Init(ListNode *list);
int List_Insert(ListNode *list, void *data);

void List_Free(ListNode *list);
ListNode *List_Get(ListNode *node, int index);

void Test_Insert(ListNode *list, int data);

int main()
{
    ListNode *root = (ListNode *)malloc(sizeof(ListNode));

    Test_Insert(root, 99);
    Test_Insert(root, 78);
    Test_Insert(root, 42);
    Test_Insert(root, 22);
    Test_Insert(root, 100);

    int *data = (int *)(List_Get(root, 3)->data);
    assert(*data == 22);
    List_Free(root);
}

void Test_Insert(ListNode *list, int data)
{
    int *addr = (int *)malloc(sizeof(int)); // int *addr is an address (pointer) to a "int" value in memory.
    *addr = data;                           // "*" here is an operator saying "addr" is a variable of type "int pointer". Access the value at that pointer. So "addr" is a pointer var and "*addr" is an int.

    int idx = List_Insert(list, (void *)addr);
    int *foundaddr = (int *)(List_Get(list, idx)->data);

    assert(*foundaddr == data);
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

    free(list->data);
    free(list);

    List_Free(next);
}
