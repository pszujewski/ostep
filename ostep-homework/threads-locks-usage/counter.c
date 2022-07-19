#define _GNU_SOURCE
#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h> /* for uint64 definition */

typedef struct timespec TimeSpec;

#define BILLION 1000000000L

#define NUMCPUS 12

#define COUNT_TO 1000000

#define NUMTHREADS 2

typedef struct __counter_t
{
    int global;                     // global count
    pthread_mutex_t glock;          // global lock
    int local[NUMCPUS];             // per-CPU count
    pthread_mutex_t llock[NUMCPUS]; // ... and locks
    int threshold;                  // update frequency

} Counter;

uint64_t get_trial_time(TimeSpec *start, TimeSpec *end);

void count(Counter *c, int threadID);

void init(Counter *c, int threshold);

void update(Counter *c, int threadID, int amt);

int get(Counter *c);

int main()
{
    TimeSpec *start = malloc(sizeof(TimeSpec));
    clock_gettime(CLOCK_REALTIME, start);

    // First, with one thread, what happens when running count()?
    // Create threads and run count()
    Counter *c = malloc(sizeof(Counter));
    init(c, 1);

    count(c, 0);
    printf("Final count => %d\n", get(c));

    free(c);

    TimeSpec *end = malloc(sizeof(TimeSpec));
    clock_gettime(CLOCK_REALTIME, end);

    uint64_t trial_time = get_trial_time(start, end);
    printf("Approximate seconds elapsed: %ld\n", trial_time);

    free(end);
    free(start);
}

uint64_t get_trial_time(TimeSpec *start, TimeSpec *end)
{
    return (end->tv_sec - start->tv_sec) + (end->tv_nsec - start->tv_nsec) / BILLION;
}

void count(Counter *c, int threadID)
{
    for (size_t i = 1; i <= COUNT_TO; i++)
    {
        update(c, threadID, 1);
    }
}

// init: record threshold, init locks, init values
// of all local counts and global count
void init(Counter *c, int threshold)
{
    c->threshold = threshold;
    c->global = 0;
    pthread_mutex_init(&c->glock, NULL);
    int i;
    for (i = 0; i < NUMCPUS; i++)
    {
        c->local[i] = 0;
        pthread_mutex_init(&c->llock[i], NULL);
    }
}

// update: usually, just grab local lock and update
// local amount; once local count has risen ’threshold’,
// grab global lock and transfer local values to it
void update(Counter *c, int threadID, int amt)
{
    int cpu = threadID % NUMCPUS;
    pthread_mutex_lock(&c->llock[cpu]);
    c->local[cpu] += amt;
    if (c->local[cpu] >= c->threshold)
    {
        // transfer to global (assumes amt>0)
        pthread_mutex_lock(&c->glock);
        c->global += c->local[cpu];
        pthread_mutex_unlock(&c->glock);
        c->local[cpu] = 0;
    }
    pthread_mutex_unlock(&c->llock[cpu]);
}

// get: just return global amount (approximate)
int get(Counter *c)
{
    pthread_mutex_lock(&c->glock);
    int val = c->global;
    pthread_mutex_unlock(&c->glock);
    return val; // only approximate!
}
