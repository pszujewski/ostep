#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

typedef struct __node_t
{
    int value;
    struct __node_t *next;

} node_t;

typedef struct __queue_t
{
    node_t *head;
    node_t *tail;
    pthread_mutex_t head_lock, tail_lock;

} queue_t;

void Queue_Init(queue_t *q)
{
    node_t *tmp = malloc(sizeof(node_t));
    tmp->next = NULL;
    q->head = q->tail = tmp;
    pthread_mutex_init(&q->head_lock, NULL);
    pthread_mutex_init(&q->tail_lock, NULL);
}

void Queue_Enqueue(queue_t *q, int value)
{
    node_t *tmp = malloc(sizeof(node_t));
    assert(tmp != NULL);
    tmp->value = value;
    tmp->next = NULL;
    pthread_mutex_lock(&q->tail_lock);
    q->tail->next = tmp;
    q->tail = tmp;
    pthread_mutex_unlock(&q->tail_lock);
}

int Queue_Dequeue(queue_t *q, int *value)
{
    pthread_mutex_lock(&q->head_lock);
    node_t *tmp = q->head;
    node_t *new_head = tmp->next;
    if (new_head == NULL)
    {
        pthread_mutex_unlock(&q->head_lock);
        return -1; // queue was empty
    }
    *value = new_head->value;
    q->head = new_head;
    pthread_mutex_unlock(&q->head_lock);
    free(tmp);
    return 0;
}

void Queue_Traverse(queue_t *q)
{
    node_t *tmp = q->tail;
    while (tmp != NULL)
    {
        printf("Value is %d\n", tmp->value);
        tmp = tmp->next;
    }
}

int main()
{
    queue_t *queue = malloc(sizeof(queue_t));
    Queue_Init(queue);
    Queue_Enqueue(queue, 1);
    Queue_Enqueue(queue, 2);
    Queue_Enqueue(queue, 3);
    Queue_Traverse(queue);
    free(queue);
}
