#include <stdio.h>
#include <getopt.h>
#include "request.h"
#include "io_helper.h"
#include "include/common_threads.h"

char default_root[] = ".";

typedef struct __worker_t
{
	pthread_t pid;
	int is_ready;
} Worker;

//
// ./wserver [-d <basedir>] [-p <portnum>]
//
int main(int argc, char *argv[])
{
	int c;
	char *root_dir = default_root;
	int port = 10000;
	int worker_threads = 1;

	while ((c = getopt(argc, argv, "d:p:t:")) != -1)
		switch (c)
		{
		case 'd':
			root_dir = optarg;
			break;
		case 'p':
			port = atoi(optarg);
			break;
		case 't':
			worker_threads = atoi(optarg);
			break;
		default:
			fprintf(stderr, "usage: wserver [-d basedir] [-p port]\n");
			exit(1);
		}

	// run out of this directory
	chdir_or_die(root_dir);

	// now, get to work
	int listen_fd = open_listen_fd_or_die(port);

	// Create a fixed-size pool of worker threads when the web server is first started.
	// pthread_t pid[worker_threads];
	// See work with semaphores in threads-sema. How can you "know" when a thread is available again to handle a request?

	while (1)
	{
		struct sockaddr_in client_addr;
		int client_len = sizeof(client_addr);

		// The main thread "waits" on the next line until what is an http request comes in.
		// "conn_fd" represents the http request.

		int conn_fd = accept_or_die(listen_fd, (sockaddr_t *)&client_addr, (socklen_t *)&client_len);

		// http request received. Put it in an enormous buffer of a fixed length
		// Each worker thread must be able to access the buffer, so ensure that you "lock" access to it.

		request_handle(conn_fd);
		close_or_die(conn_fd);
	}
	return 0;
}

/*
		create a fixed-size pool of worker threads when the web server is first started.
		Each thread is blocked until there is an http request for it to handle.
		Therefore, if there are more worker threads than active requests, then some of the threads will be blocked,
		waiting for new HTTP requests to arrive; if there are more requests than
		worker threads, then those requests will need to be buffered until there is a
		ready thread.
	*/