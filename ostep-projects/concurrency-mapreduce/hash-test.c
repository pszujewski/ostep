#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASHSIZE 1011

unsigned hash(char *s)
{
    unsigned hashval;
    for (hashval = 0; *s != '\0'; s++)
        hashval = *s + 31 * hashval;
    return hashval % HASHSIZE;
}

int main(int argc, char *argv[])
{
    printf("result %u\n", hash("cat"));
    printf("result %u\n", hash("dog"));
    printf("result %u\n", hash("Hello"));
    printf("result %u\n", hash("Michigan"));
    printf("result %u\n", hash("green"));
}