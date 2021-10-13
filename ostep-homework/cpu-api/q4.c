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
        char* argument_list[] = {"ls", "-l", NULL};
        execvp("ls", argument_list);
        printf("\n");
    }
    else
    {
        // parent path
        int rc_wait = wait(NULL);
        printf("Goodbye %d\n", rc_wait);
    }
    return 0;
}