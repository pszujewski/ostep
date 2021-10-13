#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>

int main(int argc, char *argv[])
{
    struct timeval tv;
    time_t t;
    struct tm *info;
    char buffer[64];

    gettimeofday(&tv, NULL);
    t = tv.tv_sec;

    printf("The seconds are %ld\n", t);

    info = localtime(&t);
    printf("%s", asctime(info));
    strftime(buffer, sizeof buffer, "Today is %A, %B %d.\n", info);
    printf("%s", buffer);
    strftime(buffer, sizeof buffer, "The time is %I:%M %p.\n", info);
    printf("%s", buffer);

    return 0;
}