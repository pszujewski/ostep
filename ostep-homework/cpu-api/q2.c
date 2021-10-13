#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

// https://man7.org/linux/man-pages/man2/open.2.html

int main(int argc, char *argv[])
{
    close(STDOUT_FILENO);
    open("./qa.output", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
    printf("Hello before the fork\n");
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
        printf("Hello from the child path\n");
    }
    else
    {
        // parent path
        printf("Hello from the parent path\n");
    }
}