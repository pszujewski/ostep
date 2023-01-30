#include "../ht.h"

#include <stdio.h>
#include <stdlib.h>

// gcc ./test-ht.c ../ht.c -o ./test-ht -Wall -Werror

int main()
{
    ht *people = ht_create();

    int *age = malloc(sizeof(int));

    if (age == NULL)
    {
        exit(1);
    }

    *age = 31;

    if (ht_set(people, "peter", age) == NULL)
    {
        exit(1);
    }

    void *p = ht_get(people, "peter");

    int *age_pointer = (int *)p;

    printf("Found age: %d\n", (*age_pointer));

    hti it = ht_iterator(people);

    while (ht_next(&it))
    {
        printf("%s %d\n", it.key, *(int *)it.value); // Coerce the void pointer into an int pointer and then dereference it get the value pointed to by the ptr.
        free(it.value);                              // You must free each malloc'ed "value"
    }

    ht_destroy(people);
}