#include <stdio.h>

#include "common_threads.h"

int balance = 0;

void *worker(void *arg)
{
    pthread_mutex_t lock;
    Pthread_mutex_init(&lock, NULL);
    Pthread_mutex_lock(&lock);
    balance++; // unprotected access
    Pthread_mutex_unlock(&lock);
    pthread_mutex_destroy(&lock);
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t p;
    Pthread_create(&p, NULL, worker, NULL);
    Pthread_join(p, NULL); // this thread should wait for the child thread.
    pthread_mutex_t lock;
    Pthread_mutex_init(&lock, NULL);
    Pthread_mutex_lock(&lock);
    balance++; // unprotected access
    Pthread_mutex_unlock(&lock);
    pthread_mutex_destroy(&lock);
    return 0;
}
