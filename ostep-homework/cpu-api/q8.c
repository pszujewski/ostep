#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int des_p[2];
    pipe(des_p);

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
        // char* argument_list[] = {"ls", "-l", NULL};
        // execvp("ls", argument_list);
        int inner_rc = fork();

        if (inner_rc == 0)
        {
            // inner child path
            close(STDIN_FILENO); //closing stdin
            dup(des_p[0]);       //replacing stdin with pipe read
            close(des_p[1]);     //closing pipe write
            close(des_p[0]);

            char *prog2[] = {"wc", "-l", 0};
            execvp(prog2[0], prog2);
            printf("Inner child complete\n");
            exit(1);
        }
        else
        {
            close(STDOUT_FILENO);
            dup(des_p[1]);   //replacing stdout with pipe write
            close(des_p[0]); //closing pipe read
            close(des_p[1]);
            char *prog1[] = {"ls", "-l", 0};
            execvp(prog1[0], prog1);
            printf("First child complete\n");
            exit(1);
        }
    }
    else
    {
        // parent path
        printf("Ready...\n");
    }
    close(des_p[0]);
    close(des_p[1]);
    return 0;
}