#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: ./mu [x MB] [n ms]");
        exit(1);
    }

    int mb = atoi(argv[1]);
    int secLimit = atoi(argv[2]);

    int bytes = mb * 1000000;

    int items[bytes];

    for (size_t i = 0; i < bytes; i++)
    {
        items[i] = i;
    }

    int doContinue = 1;

    clock_t start = clock(), diff;

    while (doContinue)
    {
        for (size_t i = 0; i < bytes; i++)
        {
            items[i] += 0;
        }

        diff = clock() - start;
        int sec = diff / CLOCKS_PER_SEC;

        if (sec > secLimit)
        {
            doContinue = 0;
        }
    }
}
