#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sched.h>
#include <stdint.h> /* for uint64 definition */

#define BILLION 1000000000L

typedef struct timespec TS;

size_t addAllValuesInList(int *a, size_t elementsCount);

int main(int argc, char *argv[])
{
    /* set CPU affinity to 1 core */
    cpu_set_t cpuset;

    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);
    if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset)) // if the pid is "0", the calling thread is used.
    {
        fprintf(stderr, "Error setting cpu affinity\n");
        exit(1);
    }

    long pageSize = sysconf(_SC_PAGESIZE); // _SC_PAGESIZE == 4kB == 4096 bytes

    int jump = pageSize / sizeof(int); // int == 4 bytes

    if (argc < 3)
    {
        fprintf(stderr, "Usage: ./tlb <int: pages> <int: trials>");
        exit(1);
    }

    char *numPagesToAccessArg = argv[1];
    char *numTrialsArg = argv[2];

    if (numPagesToAccessArg == NULL || numTrialsArg == NULL)
    {
        fprintf(stderr, "Usage: ./tlb <int: pages> <int: trials>");
        exit(1);
    }

    int numTrials = atoi(numTrialsArg);
    int numPagesToAccess = atoi(numPagesToAccessArg);

    int size = numPagesToAccess * jump;

    int *a = (int *)malloc(size * sizeof(int)); // Array of int

    if (a == NULL)
    {
        printf("Failed to allocate array\n");
        exit(1);
    }

    int i;
    int j;

    TS *timerStart = malloc(sizeof(TS));
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, timerStart);

    // The following loop accesses every 1024-th element in the integer array
    // Since each integer is 4 bytes large, this means you are accessing one
    // integer per memory page, since each page is 4Kb (4 * 1024 bytes) large.

    for (j = 0; j < numTrials; j++)
    {
        for (i = 0; i < size; i += jump)
        {
            a[i] = 1 + i + j; // Try to ensure compiler can't remove these loops
        }
    }

    TS *timerEnd = malloc(sizeof(TS));
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, timerEnd);

    uint64_t trialTime = BILLION * (timerEnd->tv_sec - timerStart->tv_sec) + timerEnd->tv_nsec - timerStart->tv_nsec; // Understand better

    size_t total = addAllValuesInList(a, size);
    printf("Total: %ld\n", total);

    free(a);
    free(timerEnd);
    free(timerStart);

    long averageTimePerAccess = (long)trialTime / (long)(numPagesToAccess * numTrials);
    printf("Hello! Average nanoseconds per access is %lu for a page number seed of %d\n", averageTimePerAccess, numPagesToAccess);
}

size_t addAllValuesInList(int *a, size_t elementsCount)
{
    int total = 0;
    for (size_t i = 0; i < elementsCount; i++)
    {
        total += a[i];
    }
    return total;
}