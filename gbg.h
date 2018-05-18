#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "hashmap.h"

hashmap* heap_contents;

void init();

void get_stackframe();

void* my_malloc(void* value);

int addr_comp(void* a, void* b);

void collect_garbage();
