// #include "hashmap.h"
#include "gbg.h"

/**
 * creates a hashmap struct
 * @return pointer to hashmap struct
 */
hashmap* create_hashmap() {

    // malloc
    hashmap* hm = malloc(sizeof(hashmap));
    
    // check
    if (hm == NULL) {
        return NULL;
    }

    // set default/initial values
    hm->capacity = 17;
    hm->size = 0;
    hm->buckets = malloc(sizeof(linkedlist*) * hm->capacity);
    if (hm->buckets == NULL) {
        free(hm);
        return NULL;
    }

    // initialize buckets
    for (int i = 0; i < hm->capacity; i++) {
        hm->buckets[i] = malloc(sizeof(linkedlist));
        
        // if run out of memory, free all preceding buckets
        if (hm->buckets[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(hm->buckets[j]);
            }
            free(hm->buckets);
            free(hm);
            return NULL;
        }
    }
    return hm;
}

/**
 * destroys the hashmap by free-ing everything
 * @param hm the hashmap to destroy
 */
void destroy_hashmap(hashmap* hm) {

    int cap = hm->capacity;

    // destroy each list in the hashmap
    for (int i = 0; i < cap; i++) {
        // set free_val flag to 1 to free all contents
        // of each list, as well
        destroy_list(hm->buckets[i], 1);
    }

    // free the pointer to the lists
    free(hm->buckets);

    // free the hashmap
    free(hm);
}

/**
 * insert a key-value pair into hashmap
 * @param hm      - the hashmap in which to insert the kvp
 * @param key     - the key
 * @param value   - the value
 * @param compare - function to compare keys
 * @param hash    - hash function; can be NULL if don't have one
 */
void put(hashmap* hm, void* key, void* value, int (*compare)(void*, void*), long (*hash)(void*)) {
    
    // expand hashmap if necessary
    if ((double) hm->size / (double) hm->capacity > LOAD_FACTOR) {
        expand_hm(hm, hash);
    }

    // get hashcode
    long hashcode;

    // use key's address as hashcode
    // if don't have a hash function
    if (hash == NULL) {
        char addr[10];
        sprintf(addr, "%p",key);
        hashcode = hex_to_long(addr);
    } else {
        // otherwise, use the provided
        // hashcode function
        hashcode = hash(key);
    }

    // figure out which bucket to insert into
    int bucket = hashcode % hm->capacity;

    // if are creating a new kvp
    if (!contains(hm->buckets[bucket], key, compare)) {
        add_to_tail(hm->buckets[bucket], key, value);
        hm->size += 1;
    }
    // if are updating a kvp
    else {
    	hmnode* trav = hm->buckets[bucket]->head;
    	while (trav != NULL) {
            // find the matching key
    		if (compare(trav->key, key) == 0) {
                // free the old value
                free(trav->value);
                // update to new value
    			trav->value = value;
    			return;
    		}
    		trav = trav->next;
    	}
    }
}

/**
 * remove a kvp from the hashmap
 * @param hm      - the hashmap
 * @param key     - the key
 * @param compare - comparison function pointer
 * @param hash    - hash function must be included if was used to insert
 */
void remove_from_hm(hashmap* hm, void* key, int (*compare)(void*, void*), long (*hash)(void*)) {
    
    // get hashcode    
    int hashcode;
    if (hash == NULL) {
        char addr[10];
        sprintf(addr, "%p",key);
        hashcode = hex_to_long(addr);
    } else {
        hashcode = hash(key);
    }

    // get bucket
    int bucket = hashcode % hm->capacity;

    // remove key if it actually exists
    if (contains(hm->buckets[bucket], key, compare)) {
        ll_remove(hm->buckets[bucket], key, compare);
        hm->size--;
    }
}

/**
 * function to see if hashmap contains a key
 * @param hm      - the hashmap
 * @param key     - the key
 * @param compare - comparison function pointer
 * @param hash    - hash function; optional if didn't use to insert the kvp
 */
int contains_key(hashmap* hm, void* key, int (*compare)(void*, void*), long (*hash)(void*)) {

    // get hashcode
    int hashcode;
    if (hash == NULL) {
        char addr[10];
        sprintf(addr, "%p",key);
        hashcode = hex_to_long(addr);
    } else {
        hashcode = hash(key);
    }

    // find the bucket
    int bucket = hashcode % hm->capacity;

    // run contains function on the appropriate bucket
    return contains(hm->buckets[bucket], key, compare);
}

/**
 * function to expand hashmap once load factor has been reached
 * @param hm   - the hashmap
 * @param hash - hash function pointer; must use if used to for insertion
 */
int expand_hm(hashmap* hm, long (*hash)(void*)) {

    // new capacity
    int new_cap = (hm->capacity * 2) + 1;

    // pointer to new_cap linked lists
    linkedlist** new_buckets = malloc(sizeof(linkedlist*) * new_cap);
    if (new_buckets == NULL) {
        return 0;
    }

    // initialize each linked list
    for (int i = 0; i < new_cap; i++) {
        new_buckets[i] = malloc(sizeof(linkedlist));
        if (new_buckets[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(new_buckets[j]);
            }
            free(new_buckets);
            return 0;
        }
    }

    // pull out each kvp from original linkedlist
    for (int i = 0; i < hm->capacity; i++) {
        hmnode* trav = hm->buckets[i]->head;
        while (trav != NULL) {
            
            // get hashcode
            long hashcode;
            if (hash == NULL) {
                char addr[10];
                sprintf(addr, "%p", trav->key);
                hashcode = hex_to_long(addr);
            } else {
                hashcode = hash(trav->key);
            }

            // rehash, and inseret into new linkedlist
            int bucket = hashcode % new_cap;
            add_to_tail(new_buckets[bucket], trav->key, trav->value);
            trav = trav->next;
        }
        // destroy old list
        destroy_list(hm->buckets[i], 0);
    }
    // free original linkedlist pointer
    free(hm->buckets);

    // reassign struct parameters
    hm->buckets = new_buckets;
    hm->capacity = new_cap;
    return 1;
}

/**
 * convert hex string to long
 * @param  num the hext strong
 * @return     long, decimal
 *             version of number
 */
long hex_to_long(char* num) {

    int len = strlen(num);
    
    long result = 0;
    int shift = 0;
    for (int i = len - 1; i >= 0; i--) {

        int temp;
        char c = num[i];
        if (c == 'x') {
            break;
        }
        switch (c) {
            case 'a':
                temp = 10;
                break;
            case 'b':
                temp = 11;
                break;
            case 'c':
                temp = 12;
                break;
            case 'd':
                temp = 13;
                break;
            case 'e':
                temp = 14;
                break;
            case 'f':
                temp = 15;
                break;
            default:
                temp = c - '0';
                break;
        }
        temp = temp << shift;
        result |= temp;
        shift += 4;
    }
    return result;
}

/**
 * get kvp from hashmap
 * @param hm      - the hashmap
 * @param key     - the key
 * @param compare - comparison function
 * @param hash    - hash function
 */
void* get_from_hm(hashmap* hm, void* key, int (*compare)(void*, void*), long (*hash)(void*)) {

	if (!contains_key(hm, key, compare, hash)) {
		return NULL;
	}

	long hashcode;

	if (hash == NULL) {
		char addr[20];
		sprintf(addr, "%p", key);
		hashcode = hex_to_long(addr);
	} else {
		hashcode = hash(key);
	}
	int bucket = hashcode % hm->capacity;

	linkedlist* ll = hm->buckets[bucket];

	hmnode* trav = ll->head;

	while (trav != NULL) {
		if (compare(trav->key, key) == 0) {
			return trav->value;
		}
		trav = trav->next;
	}
    return  NULL;
}

/**
 * create hashmap iterator
 * @return hashmap iterator struct
 */
hmiter* create_iter(hashmap* hm) {
    hmiter* iter = malloc(sizeof(hmiter));

    iter->bucket = -1;
    iter->bucket_iter = NULL;
    iter->kvp = NULL;
    iter->hm = hm;
    
    if (iter != NULL || hm == NULL) {
        // set values
        iter->bucket = 0;
        int i = 0;
        while (i < hm->capacity && hm->buckets[i]->head == NULL) {
            i++;
        }
        if (i < hm->capacity) {
            iter->bucket_iter = create_lliter(hm->buckets[i]);
            iter->bucket = i;
            iter->kvp = iter->bucket_iter->current;
        }
        iter->hm = hm;
    }

    return iter;
}

void destroy_iter(hmiter* iter) {
    if (iter == NULL) {
        return;
    }
    if (iter->bucket_iter != NULL) {
        free(iter->bucket_iter);
    }
    free(iter);
}

/**
 * iterate over hashmap
 * @param  hm   the hashmap
 * @param  iter the iterator
 */
void iterate(hmiter* iter) {

    // store contents of iterator locally
    int capacity = iter->hm->capacity;
    int bucket = iter->bucket;
    lliter* b_iter = iter->bucket_iter;

    int current_bucket = 1;

    while (bucket < capacity) {

        // iterate only if haven't changed buckets
        if (current_bucket) {
            ll_iterate(b_iter);
        }

        // if current item that's being iterated on
        // is null
        if (b_iter->current == NULL) {
            
            // increment bucket
            bucket++;

            if (bucket < capacity) {
                // free() current iterator and create new one
                free(b_iter);
                b_iter = create_lliter(iter->hm->buckets[bucket]);
            }

            // set flag
            current_bucket = 0;
        } else {
            // store data and return
            iter->bucket = bucket;
            iter->bucket_iter = b_iter;
            iter->kvp = b_iter->current;
            return;
        }
    }
    
    // store data, as well
    iter->bucket = bucket;
    iter->bucket_iter = b_iter;
    iter->kvp = b_iter->current;
}

void print_hm(hashmap* hm) {

    hmiter* iter = create_iter(hm);

    while (iter->kvp != NULL) {

        hmnode* kvp = iter->kvp;

        long* key = (long*)kvp->key;
        meta* val = (meta*)kvp->value;

        printf("0x%lx : size = %zu; marked = %d\n", *key, val->size, val->marked);

        iterate(iter);
    }    


}