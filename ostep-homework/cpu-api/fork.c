#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int x = 100;
    printf("hello from the root\n");
    int rc = fork();

    if (rc < 0)
    {
        // fork failed
        fprintf(stderr, "fork failed\n");
        exit(1);
    }
    else if (rc == 0)
    {
        // child path
        x = x * 2;
        printf("x in child is %d\n", x);
    }
    else
    {
        // parent path
        printf("x in parent is %d\n", x);
    }
}