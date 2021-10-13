#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>

time_t get_time_ms()
{
    struct timeval tv;
    time_t t;
    struct tm *info;
    char buffer[64];

    gettimeofday(&tv, NULL);
    t = tv.tv_sec;
    return t;
}

void read_arbitrary_bytes()
{
    char buf[20];
    size_t nbytes;
    ssize_t bytes_read;
    int fd = open("./ex.txt", O_RDWR);
    nbytes = sizeof(buf);
    bytes_read = read(fd, buf, nbytes);
    printf("bytes %ld\n", bytes_read);
}

int main(int argc, char *argv[])
{
    int count = 50;
    time_t t1 = get_time_ms();

    for (int i = 0; i < count; i++)
    {
        read_arbitrary_bytes();
    }

    time_t t2 = get_time_ms();
    time_t difference = t2 - t1;

    printf("The diff is %ld\n", difference);
    return 0;
}