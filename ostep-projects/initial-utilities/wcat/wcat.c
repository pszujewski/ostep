#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        exit(0);
    }

    for (int i = 1; i < argc; i++)
    {
        FILE *fp = fopen(argv[i], "r");

        if (fp == NULL)
        {
            printf("wcat: cannot open file\n");
            exit(1);
        }

        char s[150];

        while (fgets(s, 150, fp))
        {
            printf("%s", s);
        }

        fclose(fp);
    }

    return 0;
}