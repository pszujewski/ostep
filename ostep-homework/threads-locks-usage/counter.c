#define _GNU_SOURCE
#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h> /* for uint64 definition */

typedef struct timespec TimeSpec;

#define BILLION 1000000000L

uint64_t get_trial_time(TimeSpec *start, TimeSpec *end);

int main()
{
    TimeSpec *start = malloc(sizeof(TimeSpec));
    clock_gettime(CLOCK_REALTIME, start);

    sleep(3);

    TimeSpec *end = malloc(sizeof(TimeSpec));
    clock_gettime(CLOCK_REALTIME, end);

    uint64_t trial_time = get_trial_time(start, end);
    printf("Approximate seconds elapsed: %ld\n", trial_time);
}

uint64_t get_trial_time(TimeSpec *start, TimeSpec *end)
{
    return (end->tv_sec - start->tv_sec) + (end->tv_nsec - start->tv_nsec) / BILLION;
}
