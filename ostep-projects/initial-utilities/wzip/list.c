// Sources:
// https://stackoverflow.com/questions/3536153/c-dynamically-growing-array
// https://medium.com/@patdhlk/c-dynamic-data-structures-1-7e518b2868a0

#include "list.h"

list_node *list_init()
{
    return node_init();
}

list_node *node_init()
{
    list_node *listp = (list_node *)malloc(sizeof(list_node));
    if (listp == NULL)
    {
        return NULL;
    }
    listp->index = 0;
    listp->next = NULL;
    listp->data = NULL;
    return listp;
}

size_t list_insert(list_node *lst, void *datap)
{
    list_node *last_node = get_last_node(lst);

    if (last_node->data == NULL)
    {
        last_node->data = datap;
        return last_node->index;
    }

    list_node *listp = node_init();

    listp->next = NULL;
    listp->data = datap;
    listp->index = last_node->index + 1;

    last_node->next = listp;

    return listp->index;
}

list_node *get_node(list_node *lst, size_t index)
{
    list_node *current = lst;

    while (current->index != index)
    {
        current = current->next;

        if (current == NULL)
        {
            return NULL; // Not found
        }
    }

    return current;
}

list_node *get_last_node(list_node *lst)
{
    if (lst->next == NULL)
    {
        return lst;
    }
    return get_last_node(lst->next);
}

void list_free(list_node *lst)
{
    list_node *next = lst->next;

    if (lst->data != NULL)
    {
        free(lst->data);
    }

    free(lst);

    if (next != NULL)
    {
        list_free(next);
    }
}
