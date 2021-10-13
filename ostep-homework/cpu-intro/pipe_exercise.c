#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

/*
a pipe is just one of many ways
processes in a UNIX system can communicate with one another.
*/

/*
- There will be two processes plus the "parent" process which is kicked off by main() 
- I think you will need two distinct pipes. 
- The first process issues a "write" to the first pipe and waits for a "read" on the second
- The second process reads from the first pipe and then writes to the second
- To ensure all this code runs on a single processor, use sched setaffinity()
*/

/*
On writing/reading pipes:
https://stackoverflow.com/questions/47503798/write-on-pipe-in-c

On closing file descriptors: 
https://stackoverflow.com/questions/15196784/how-to-wait-till-data-is-written-on-the-other-end-of-pipe
*/

int main(int argc, char *argv[])
{
    // cpu_set_t mask;

    // CPU_ZERO(&mask);
    // CPU_SET(0, &mask);

    // if (sched_setaffinity(0, sizeof(cpu_set_t), &mask) == -1)
    // {
    //     perror("sched_setaffinity");
    // }

    int des_p1[2];
    pipe(des_p1);

    int pipe1_fd_in = des_p1[1];  // write to
    int pipe1_fd_out = des_p1[0]; // read from

    int des_p2[2];
    pipe(des_p2);

    int pipe2_fd_in = des_p2[1];  // write to
    int pipe2_fd_out = des_p2[0]; // read from

    int rc = fork();

    if (rc < 0)
    {
        // fork failed
        fprintf(stderr, "fork failed\n");
        exit(1);
    }
    else if (rc == 0)
    {
        // Start of first child process
        int inner_rc = fork();

        if (inner_rc == 0)
        {
            // Inner (second) child process
            // Close the file descriptors not needed here
            close(STDIN_FILENO);
            close(pipe1_fd_in);
            close(pipe2_fd_out);
            // read from the first pipe
            char message_rcvd[3];
            read(pipe1_fd_out, message_rcvd, strlen(message_rcvd) + 1);
            printf("Inner child received message: %s\n", message_rcvd);
            // then write to the second
            char message_to_send[6] = {'P', 'e', 't', 'e', 'r', '\0'};
            write(pipe2_fd_in, message_to_send, strlen(message_to_send) + 1);
        }
        else
        {
            // First child process ONLY
            // Close the file descriptors not needed here
            close(STDIN_FILENO);
            close(pipe1_fd_out);
            close(pipe2_fd_in);
            // Message
            char message[3] = {'P', 'e', '\0'};
            // issue a "write" to the first pipe
            write(pipe1_fd_in, message, strlen(message) + 1);
            printf("First child to wait...\n");
            // wait for the inner child to complete first
            wait(NULL);
            // pipe the read to stdout and print your name
            char final_msg[6];
            read(pipe2_fd_out, final_msg, strlen(final_msg) + 1);
            printf("First child received message: %s\n", final_msg);
        }
    }
    else
    {
        // Parent process
        // Close the file descriptors not needed here
        close(pipe1_fd_in);
        close(pipe2_fd_out);
        close(pipe1_fd_out);
        close(pipe2_fd_in);
        wait(NULL);
        printf("Children processes done...\n");
    }
}