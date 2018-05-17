#include "hashmap.h"

hashmap* create_hashmap() {
    hashmap* hm = malloc(sizeof(hashmap));
    if (hm == NULL) {
        return NULL;
    }
    hm->capacity = 16;
    hm->size = 0;
    hm->buckets = malloc(sizeof(linkedlist*) * hm->capacity);
    if (hm->buckets == NULL) {
        free(hm);
        return NULL;
    }
    for (int i = 0; i < hm->capacity; i++) {
        hm->buckets[i] = malloc(sizeof(linkedlist));
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

void put(hashmap* hm, void* key, void* value, int (*compare)(void*, void*), int (*hash)(void*)) {
    
    if ((double) hm->size / (double) hm->capacity > LOAD_FACTOR) {
        expand_hm(hm, hash);
    }

    int hashcode;

    if (hash == NULL) {
        char addr[10];
        sprintf(addr, "%p",key);
        hashcode = hex_to_int(addr);
    } else {
        hashcode = hash(key);
    }

    int bucket = hashcode % hm->capacity;

    if (!contains(hm->buckets[bucket], key, compare)) {
        add_to_tail(hm->buckets[bucket], key, value);
        hm->size++;
    } else {
	hmnode* trav = hm->buckets[bucket]->head;
	while (trav != NULL) {
		if (compare(trav->key, key)) {
			free(trav->value);
			trav->value = value;
			return;
		}
		trav = trav->next;
	}
    }
}

void remove_from_hm(hashmap* hm, void* key, int (*compare)(void*, void*), int (*hash)(void*)) {
    
    int hashcode;
    if (hash == NULL) {
        char addr[10];
        sprintf(addr, "%p",key);
        hashcode = hex_to_int(addr);
    } else {
        hashcode = hash(key);
    }

    int bucket = hashcode % hm->capacity;

    if (!contains(hm->buckets[bucket], key, compare)) {
        ll_remove(hm->buckets[bucket], key, compare);
        hm->size--;
    }
}

int contains_key(hashmap* hm, void* key, int (*compare)(void*, void*), int (*hash)(void*)) {

    int hashcode;
    if (hash == NULL) {
        char addr[10];
        sprintf(addr, "%p",key);
        hashcode = hex_to_int(addr);
    } else {
        hashcode = hash(key);
    }

    int bucket = hashcode % hm->capacity;

    return contains(hm->buckets[bucket], key, compare);
}

int expand_hm(hashmap* hm, int (*hash)(void*)) {

    int new_cap = (hm->capacity * 2) + 1;

    linkedlist** new_buckets = malloc(sizeof(linkedlist*) * new_cap);
    if (new_buckets == NULL) {
        return 0;
    }
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
    for (int i = 0; i < hm->capacity; i++) {
        hmnode* trav = hm->buckets[i]->head;
        while (trav != NULL) {
            int hashcode;
            if (hash == NULL) {
                char addr[10];
                sprintf(addr, "%p", trav->key);
                hashcode = hex_to_int(addr);
            } else {
                hashcode = hash(trav->key);
            }
            int bucket = hashcode % new_cap;
            add_to_tail(new_buckets[i], trav->key, trav->value);
            trav = trav->next;
        }
        destroy_list(hm->buckets[i], 0);
    }
    free(hm->buckets);
    hm->buckets = new_buckets;
    hm->capacity = new_cap;
    return 1;
}

int hex_to_int(char* num) {

    int len = strlen(num);
    
    int result = 0;
    int shift = 0;
    for (int i = len - 1; i > 1; i--) {
        int temp;
        char c = num[i];
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

void* get_from_hm(hashmap* hm, void* key, int (*compare)(void*, void*), int (*hash)(void*)) {

	if (!contains_key(hm, key, compare, hash)) {
		return NULL;
	}

	int hashcode;

	if (hash == NULL) {
		char addr[20];
		sprintf(addr, "%p", key);
		hashcode = hex_to_int(addr);
	} else {
		hashcode = hash(key);
	}
	int bucket = hashcode % hm->capacity;

	linkedlist* ll = hm->buckets[bucket];

	hmnode* trav = ll->head;

	while (trav != NULL) {
		if (compare(trav->key, key)) {
			return trav->value;
		}
		trav = trav->next;
	}
    return  NULL;
}

hmiter* create_iter() {
    hmiter* iter = malloc(sizeof(hmiter));
    
    if (iter != NULL) {
        iter->bucket = 0;
        iter->kvp = NULL;
    }

    return iter;
}

hmiter* iterate(hashmap* hm, hmiter* iter) {

    if (hm == NULL) {
        return  NULL;
    }

    int bucket = iter->bucket;
    int count = iter->count;
    hmnode* kvp = iter->kvp;

    if (count == hm->size) {
        return NULL;
    }

    if (kvp == NULL) {
        while (kvp == NULL) {
            kvp = hm->buckets[bucket++]->head;
        }
    } else {
        kvp = kvp->next;
        while (kvp == NULL) {
            kvp = hm->buckets[bucket++]->head;
        }
    }
    iter->kvp = kvp;
    iter->bucket = bucket;
    iter->count += 1;
    
    return iter;
}
