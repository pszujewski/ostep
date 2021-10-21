#ifndef _LST_H
#define _LST_H

#include <stdlib.h>

typedef struct list_node
{
    size_t index;
    void *data;
    struct list_node *next;
} list_node;

list_node *list_init();

list_node *node_init();

size_t list_insert(list_node *lst, void *datap);

list_node *list_get(list_node *lst, size_t index);

list_node *get_last_node(list_node *lst);

void list_free(list_node *lst);

#endif // _LST_H
