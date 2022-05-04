#include <unistd.h>
#include <stdio.h>

// _SC_PAGESIZE == 4kB == 4096 bytes

int main(int argc, char *argv[])
{
    long sz = sysconf(_SC_PAGESIZE);
    printf("Page size: %ld bytes\n", sz);
}