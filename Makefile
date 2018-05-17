CC = clang
ARGS = -Wall -g

all: gbg

gbg: gbg.c
	$(CC) -o gbg $(ARGS) gbg.c

clean:
	rm -rf *.o gbg