#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "hashmap.h"

typedef struct Metadata meta;
struct Metadata {
    // long addr;
    int size;
    int marked;
};


hashmap* heap_contents;

void init();

linkedlist* get_stackframe(int frame_backup);

void* my_malloc(size_t size);

int addr_comp(void* a, void* b);

void collect_garbage();

void pre_mark();

void mark_contents(void* ptr);

void discard_unmarked();

int is_ptr(void* val);
