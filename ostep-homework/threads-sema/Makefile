
BINARIES = fork-join rendezvous barrier reader-writer reader-writer-nostarve
HEADERS = common_threads.h

all: $(BINARIES)

clean:
	rm -f $(BINARIES)

fork-join: fork-join.c $(HEADERS)
	gcc -o fork-join fork-join.c -Wall -pthread

rendezvous: rendezvous.c $(HEADERS)
	gcc -o rendezvous rendezvous.c -Wall -pthread

barrier: barrier.c $(HEADERS)
	gcc -o barrier barrier.c -Wall -pthread

reader-writer: reader-writer.c $(HEADERS)
	gcc -o reader-writer reader-writer.c -Wall -pthread

reader-writer-nostarve: reader-writer-nostarve.c $(HEADERS)
	gcc -o reader-writer-nostarve reader-writer-nostarve.c -Wall -pthread




