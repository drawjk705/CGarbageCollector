#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "linkedlist.h"
#define LOAD_FACTOR 0.7

typedef struct HashMap hashmap;
struct HashMap {

	linkedlist** buckets;
	int capacity;
	int size;

};

typedef struct HM_Iter hmiter;
struct HM_Iter
{
    int bucket;
    int count;
    hmnode* kvp;
};

hashmap* create_hashmap();

void destroy_hashmap(hashmap* hm);

void put(hashmap* hm, void* key, void* value, int (*compare)(void*, void*), int (*hash)(void*));

void remove_from_hm(hashmap* hm, void* key, int (*compare)(void*, void*), int (*hash)(void*));

int contains_key(hashmap* hm, void* key, int (*compare)(void*, void*), int (*hash)(void*));

int expand_hm(hashmap* hm, int (*hash)(void*));

int hex_to_long(char* num);

void* get_from_hm(hashmap* hm, void* key, int (*compare)(void*, void*), int (*hash)(void*));

hmiter* create_iter();

hmiter* iterate(hashmap* hm, hmiter* iter);

void print_hm(hashmap* hm);
