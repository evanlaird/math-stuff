# Environment
CC=gcc
CFLAGS=-Wall -O0 -g

all:ising-runner

ising-runner: runtime.o datatypes.o array.o
	$(CC) $(CFLAGS) runtime.o datatypes.o array.o -o ising-runner

runtime.o: runtime.c
	$(CC) $(CFLAGS) -c runtime.c

datatypes.o: datatypes.c
	$(CC) $(CFLAGS) -c datatypes.c

array.o: array.c
	$(CC) $(CFLAGS) -c array.c

clean:
	rm -rf *.o ising-runner
