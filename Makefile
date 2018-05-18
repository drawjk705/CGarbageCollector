CC = clang
ARGS = -Wall -g

all: gbg hashmap linkedlist

hashmap: hashmap.c
	$(CC) -c $(ARGS) hashmap.c


linkedlist: linkedlist.c
	$(CC) -c $(ARGS) linkedlist.c

gbg: gbg.c hashmap linkedlist
	$(CC) -o gbg $(ARGS) gbg.c hashmap.o linkedlist.o

clean:
	rm -rf *.o gbg