#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct __list_node_t
{
    int index;
    ListNode *next;
    void *data
} ListNode;

void main()
{
    ListNode *root = malloc(sizeof(ListNode));
    List_Insert(root, (void *)100);
    List_Insert(root, (void *)99);
    List_Insert(root, (void *)78);
    List_Insert(root, (void *)42);
    List_Insert(root, (void *)22);
    assert(List_Get(root, 3)->data == 42);
    List_Free(root);
}

void List_Init(ListNode *list)
{
    list->index = 0;
    list->next = NULL;
    list->data = NULL;
}

int List_Insert(ListNode *list, void *data)
{
    int index = 0;

    if (list->data == NULL)
    {
        list->data = data;
    }
    else
    {
        ListNode *child = list->next;

        while (child != NULL)
        {
            child = child->next;
            index++;
        }

        ListNode *node = malloc(sizeof(ListNode));
        List_Init(node);

        node->data = data;
        node->index = index;

        child = node;
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
    free(list);

    List_Free(next);
}
