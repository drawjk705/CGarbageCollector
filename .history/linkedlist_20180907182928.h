#include <stdlib.h>
#include <stdio.h>

typedef struct HMNode hmnode;
struct HMNode {
	void* key;
	void* value;
	hmnode* next;
};

// hmnode constructor
hmnode* create_node(void* key, void* value);

// hmnode destructor
// with 'optional' destructor for the actual node itself
void destroy_node(hmnode* n, int free_val, void (*node_destructor)(void*));

typedef struct LinkedList linkedlist;
struct LinkedList {
	hmnode* head;
};

linkedlist* create_ll();

void add_to_head(linkedlist* ll, void* key, void* value);
void add_to_tail(linkedlist* ll, void* key, void* value);
int contains(linkedlist* ll, void* key, int (*compare)(void*, void*));
void ll_remove(linkedlist* ll, void* key, int (*compare)(void*, void*));
void destroy_list(linkedlist* ll, int free_val, void (*node_destructor)(void*));
void print_ll(linkedlist* ll);


typedef struct LLIter lliter;
struct LLIter {
    hmnode* current;
    int has_next;
};

lliter* create_lliter(linkedlist* ll);

void ll_iterate(lliter* iter);
