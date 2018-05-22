#include <stdlib.h>
#include <stdio.h>

typedef struct HMNode hmnode;
struct HMNode {
	void* key;
	void* value;
	hmnode* next;
};

/*
typedef struct Node node;
struct Node {
	void* value;
	node* next;
};
*/

hmnode* create_node(void* key, void* value);

void destroy_node(hmnode* n, int free_val);

typedef struct LinkedList linkedlist;
struct LinkedList {
	hmnode* head;
};

linkedlist* create_ll();

void add_to_head(linkedlist* ll, void* key, void* value);
void add_to_tail(linkedlist* ll, void* key, void* value);
int contains(linkedlist* ll, void* key, int (*compare)(void*, void*));
void ll_remove(linkedlist* ll, void* key, int (*compare)(void*, void*));
void destroy_list(linkedlist* ll, int free_val);
void print_ll(linkedlist* ll);


typedef struct LLIter lliter;
struct LLIter {
    hmnode* current;
    int has_next;
};

lliter* create_lliter();

lliter* ll_iterate(linkedlist* ll, lliter* iter);

lliter* reset_iter(linkedlist* ll, lliter* iter);
