#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

FILE *get_in(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("wgrep: searchterm [file ...]\n");
        exit(1);
    }

    char *term = argv[1];
    FILE *fp = get_in(argc, argv);

    if (fp == NULL)
    {
        printf("wgrep: cannot open file\n");
        exit(1);
    }

    char s[150];

    while (fgets(s, 150, fp))
    {
        if (strstr(s, term))
        {
            printf("%s", s);
        }
    }

    fclose(fp);
    return 0;
}

FILE *get_in(int argc, char *argv[])
{
    if (argc > 2)
    {
        return fopen(argv[2], "r");
    }
    return stdin;
}