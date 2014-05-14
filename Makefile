# Environment
CC=gcc
CFLAGS=-Wall -O0 -lm

all:ising-runner

ising-runner: runtime.o datatypes.o 
	$(CC) runtime.o datatypes.o -o ising-runner -lm -O0

runtime.o: runtime.c
	$(CC) $(CFLAGS) -c runtime.c

datatypes.o: datatypes.c
	$(CC) $(CFLAGS) -c datatypes.c

clean:
	rm -rf *.o ising-runner
