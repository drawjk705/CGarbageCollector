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

// constructor
hashmap* create_hashmap();

// destructor
void destroy_hashmap(hashmap* hm);

// add to hashmap
void put(hashmap* hm, void* key, void* value, int (*compare)(void*, void*), long (*hash)(void*));

// remove from hashmap
void remove_from_hm(hashmap* hm, void* key, int (*compare)(void*, void*), long (*hash)(void*));

// check for membership in hashmap
int contains_key(hashmap* hm, void* key, int (*compare)(void*, void*), long (*hash)(void*));

// "private" function for maintenance
int expand_hm(hashmap* hm, long (*hash)(void*));

// convert hex representation of number
// (as a string) to long
long hex_to_long(char* num);

void* get_from_hm(hashmap* hm, void* key, int (*compare)(void*, void*), long(*hash)(void*));


/**
 * hashmap iterator struct definition
 */
typedef struct HM_Iter hmiter;
struct HM_Iter
{
    int bucket;
    lliter* bucket_iter;
    hmnode* kvp;
    hashmap* hm;
};

hmiter* create_iter(hashmap* hm);

void iterate(hmiter* iter);

void print_hm(hashmap* hm);
