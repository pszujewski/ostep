#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include "./ht.h"

// Run ./wzip ./tests/4.in
// Rather than immediately writing the int/char combo to the stdout, add them to a list that you write at once to stdout.

typedef struct
{
    int *array;
    size_t used;
    size_t size;
} Array;

void test();

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

void process_file(char *path, Array *a);

void aggregate_results(Array a);

void loop_over(Array a);

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("wzip: file1 [file2 ...]\n");
        exit(1);
    }

    Array a;
    initArray(&a, 5);

    for (int i = 1; i < argc; i++)
    {
        process_file(argv[i], &a);
    }

    if (argc > 2)
    {
        //printf("agg_res");
        aggregate_results(a);
    }
    else
    {
        loop_over(a);
    }

    freeArray(&a);
    return 0;
}

void process_file(char *path, Array *pa)
{
    FILE *fp = fopen(path, "r");

    if (fp == NULL)
    {
        printf("wzip: cannot open file\n");
        exit(1);
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, fp)) != -1)
    {
        char found = 0;
        int count = 0;
        //printf("line is of length: %ld", len);

        for (int i = 0; i < nread; i++)
        {
            if (isalpha(line[i]) && found == 0)
            {
                //printf("Init alpha found: %c\n", line[i]);
                found = line[i];
                count++;
            }
            else if (isalpha(line[i]) && found == line[i])
            {
                //printf("INCREMENTING\n");
                count++;
            }
            else if (isalpha(line[i]))
            {
                printf("alpha found: %c\n", line[i]);
                insertArray(pa, count);
                insertArray(pa, (int)found);
                count = 1;
                found = line[i];
            }
            else if (isspace(line[i]))
            {
                insertArray(pa, count);
                insertArray(pa, (int)found);
                count = 1;
                found = line[i];
            }
        }
        //printf("%d\n", count);
        //printf("%c\n", (int)found);
        insertArray(pa, count);
        insertArray(pa, (int)found);
    }

    free(line);
    fclose(fp);
}

void aggregate_results(Array a)
{
    ht *counts_by_alpha = ht_create();

    int curr_count = 0;

    for (size_t i = 0; i < a.used; i++)
    {
        bool isEven = i % 2 == 0;

        if (isEven)
        {
            curr_count = a.array[i];
        }
        else
        {
            char key[2] = "\0";
            key[0] = (char)a.array[i];
            void *p = ht_get(counts_by_alpha, key);

            if (p != NULL)
            {
                int *data_pointer = (int *)p;
                (*data_pointer) += curr_count;
            }
            else
            {
                int *data = malloc(sizeof(int));

                if (data == NULL)
                {
                    exit(1);
                }

                *data = curr_count;

                if (ht_set(counts_by_alpha, key, data) == NULL)
                {
                    printf("ht_set: failed to set value in table");
                    exit(1);
                }
                //printf("Data to insert: %d\n", (*data));
            }
        }
    }

    hti it = ht_iterator(counts_by_alpha);

    while (ht_next(&it))
    {
        char c = it.key[0];
        fwrite((int *)it.value, sizeof(int), 1, stdout);
        fwrite(&c, sizeof(char), 1, stdout);
        free(it.value);
    }

    ht_destroy(counts_by_alpha);
}

void loop_over(Array a)
{
    for (size_t i = 0; i < a.used; i++)
    {
        //printf("Inspecting used again: %ld\n", a.used);
        bool isEven = i % 2 == 0;
        int current = a.array[i];

        //printf("Element index: %ld\n", i);
        if (isEven)
        {
            //printf("%d\n", current);
            fwrite(&current, sizeof(int), 1, stdout);
        }
        else
        {
            //printf("%c\n", current);
            fwrite(&current, sizeof(char), 1, stdout);
        }
    }
}