CC = gcc
OPTS = -Wall -Werror -g -I./include

all:
	$(CC) -pthread ./counter.c -o ./counter $(OPTS)

# run with "make clean"
clean:
	rm -rf ./counter