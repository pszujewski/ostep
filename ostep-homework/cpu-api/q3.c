#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
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
        printf("Hello\n");
    }
    else
    {
        // parent path
        wait(NULL);
        printf("Goodbye\n");
    }
    return 0;
}