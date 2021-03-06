// From
// https://stackoverflow.com/questions/3536153/c-dynamically-growing-array

#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int *array;
    size_t used;
    size_t size;
} Array;

void initArray(Array *a, size_t initialSize)
{
    a->array = malloc(initialSize * sizeof(int));
    a->used = 0;
    a->size = initialSize;
}

void insertArray(Array *a, int element)
{
    if (a->used == a->size)
    {
        a->size *= 2;
        a->array = realloc(a->array, a->size * sizeof(int));
    }
    a->array[a->used++] = element;
}

void freeArray(Array *a)
{
    free(a->array);
    a->array = NULL;
    a->used = a->size = 0;
}

void test()
{
    Array a;
    int i;

    initArray(&a, 5); // initially 5 elements
    for (i = 0; i < 100; i++)
        insertArray(&a, i);     // automatically resizes as necessary
    printf("%d\n", a.array[9]); // print 10th element
    printf("%ld\n", a.used);    // print number of elements
    freeArray(&a);
}

int main()
{
    test();
}
