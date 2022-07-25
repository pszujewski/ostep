CC = gcc
OPTS = -Wall -Werror -g -I./include

all:
	$(CC) -pthread ./counter.c -o ./counter $(OPTS)

ll: 
	$(CC) ./linked-list.c -o ./ll $(OPTS)

# run with "make clean"
clean:
	rm -rf ./counter ./ll