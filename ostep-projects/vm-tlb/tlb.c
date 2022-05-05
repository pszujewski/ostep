#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define BILLION 1000000000.0

int main(int argc, char *argv[])
{
    long PAGESIZE = sysconf(_SC_PAGESIZE); // _SC_PAGESIZE == 4kB == 4096 bytes

    int jump = PAGESIZE / sizeof(int); // int == 4 bytes

    if (argc == 1)
    {
        exit(1);
    }

    int const NUMPAGES = (int)argv[1][0];
    int list[NUMPAGES * PAGESIZE * 2];

    int limit = NUMPAGES * PAGESIZE;
    int listAccessCount = 0;

    long microsecondsTotal = 0;

    int i;
    struct timeval start, end;

    // Given the NUMPAGES
    // Find the average time each access of the array takes.
    // You should see it dramatically increase as the NUMPAGES goes up a lot.

    for (i = 0; i < limit; i += jump)
    {
        gettimeofday(&start, NULL);
        list[i] += 1;

        listAccessCount++;
        gettimeofday(&end, NULL);

        long seconds = (end.tv_sec - start.tv_sec);
        long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);

        microsecondsTotal += micros;
    }

    float averageTimePerAccess = (float)microsecondsTotal / (float)listAccessCount;
    printf("Average microseconds per access is %f for a page number seed of %d\n", averageTimePerAccess, NUMPAGES);
    // https://www.techiedelight.com/find-execution-time-c-program/
    // https://stackoverflow.com/questions/13772567/how-to-get-the-cpu-cycle-count-in-x86-64-from-c/64898073#64898073
}