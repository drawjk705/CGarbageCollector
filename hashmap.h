#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "linkedlist.h"
#define LOAD_FACTOR 0.7

/**
 * hashmap struct definition
 */
typedef struct HashMap hashmap;
struct HashMap {

	linkedlist** buckets;
	int capacity;
	int size;

};


hashmap* create_hashmap();

void destroy_hashmap(hashmap* hm);

void put(hashmap* hm, void* key, void* value, int (*compare)(void*, void*), int (*hash)(void*));

void remove_from_hm(hashmap* hm, void* key, int (*compare)(void*, void*), int (*hash)(void*));

int contains_key(hashmap* hm, void* key, int (*compare)(void*, void*), int (*hash)(void*));

int expand_hm(hashmap* hm, int (*hash)(void*));

long hex_to_long(char* num);

void* get_from_hm(hashmap* hm, void* key, int (*compare)(void*, void*), int (*hash)(void*));


/**
 * hashmap iterator struct definition
 */
typedef struct HM_Iter hmiter;
struct HM_Iter
{
    int bucket;
    int count;
    int has_next;
    hmnode* kvp;
};

hmiter* create_iter();

hmiter* iterate(hashmap* hm, hmiter* iter);

hmiter* reset_iter_hm(hashmap* hm, hmiter* iter);

void print_hm(hashmap* hm);
