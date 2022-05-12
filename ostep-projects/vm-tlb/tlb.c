#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>	/* for uint64 definition */

#define BILLION 1000000000L

int main(int argc, char *argv[])
{
    long PAGESIZE = sysconf(_SC_PAGESIZE); // _SC_PAGESIZE == 4kB == 4096 bytes

    int jump = PAGESIZE / sizeof(int); // int == 4 bytes

    if (argc == 1)
    {
        exit(1);
    }

    char *numPagesArg = argv[1];

    int const NUMPAGES = atoi(numPagesArg);
    int list[(NUMPAGES * PAGESIZE) + jump];

    int limit = NUMPAGES * PAGESIZE;
    uint64_t listAccessCount = 0;

    int i;

    struct timespec timerStart;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timerStart);

    for (i = 0; i < limit; i += jump)
    {
        list[i] += 1;
        listAccessCount++;
    }

    struct timespec timerEnd;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timerEnd);
    
    uint64_t diff = BILLION * (timerEnd.tv_sec - timerStart.tv_sec) + timerEnd.tv_nsec - timerStart.tv_nsec;

    long ns = (long long unsigned int)diff;
    long accesses = (long long unsigned int)listAccessCount;

    long averageTimePerAccess = ns / accesses;
    printf("Hello! Average nanoseconds per access is %lu for a page number seed of %d\n", averageTimePerAccess, NUMPAGES);
}