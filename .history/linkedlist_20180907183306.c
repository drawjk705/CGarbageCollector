#include "linkedlist.h"


hmnode* create_node(void* key, void* value) {
    hmnode* n = malloc(sizeof(hmnode));

    if (n == NULL) {
         return NULL;
    }

    n->key = key;
    n->value = value;
    n->next = NULL;
    return n;
}

void destroy_node(hmnode* n, int free_contents, void (*key_destructor)(void*), void(*val_destructor)(void*)) {
    if (free_contents) {
        if (n->key != NULL) {
			(key_destructor == NULL) ? free(n->key) : key_destructor(n->key);
            free(n->key);
        }
        if (n->value != NULL) {
            free(n->value);
        }
    }
    free(n);
}

/*################################*/

linkedlist* create_ll() {
    linkedlist* ll = malloc(sizeof(linkedlist));
    return ll;
}

void add_to_head(linkedlist* ll, void* key, void* value) {

        if (ll == NULL) {
              return;
        }
        hmnode* n = create_node(key, value);
        if (n == NULL) {
            return;
        }
        ll->head = n;
}

void add_to_tail(linkedlist* ll, void* key, void* value) {
	
	if (ll == NULL) {
		return;
	}

	if (ll->head == NULL) {
		add_to_head(ll, key, value);
		return;
	}
	hmnode* trav = ll->head;
	while (trav->next != NULL) {
		trav = trav->next;
	}
	trav->next = create_node(key, value);

}

int contains(linkedlist* ll, void* key, int (*compare)(void*, void*)) {

	if (ll == NULL) {
		return 0;
	}

	hmnode* trav = ll->head;

	while (trav != NULL) {
		if (compare(trav->key, key) == 0) {
			return 1;
		}
		trav = trav->next;
	}

	return 0;
}

void ll_remove(linkedlist* ll, void* key, int (*compare)(void*, void*)) {

	if (ll == NULL) {
		return;
	}

	hmnode* trav = ll->head;
	hmnode* prev;

	while (trav != NULL) {
		if (compare(&(trav->key), &key)) {
			if (trav == ll->head) {
				ll->head = trav->next;
			} else {
				prev->next = trav->next;
			}
			destroy_node(trav, 1);
			return;
		}
		prev = trav;
		trav = trav->next;
	}
}

void destroy_list(linkedlist* ll, int free_val) {

	if (ll == NULL) {
		return;
	}

	hmnode* trav = ll->head;
    if (trav == NULL) {
        free(ll);
        return;
    }


	hmnode* temp;

	while (trav->next != NULL) {
		temp = trav->next;
		destroy_node(trav, free_val);
		trav = temp;
	}
	free(ll);
}

lliter* create_lliter(linkedlist* ll) {

    lliter* iter = malloc(sizeof(lliter));

    if (iter != NULL) {
        iter->current = ll->head;
        iter->has_next = 0;
    }

    return iter;
}

void ll_iterate(lliter* iter) {

    if (iter == NULL) {
        return;
    }

    if (iter->current == NULL) {
        iter->current = NULL;
        return;
    }

    iter->current = iter->current->next;
}

void print_ll(linkedlist* ll) {

    lliter* iter = create_lliter(ll);

    while (iter->current != NULL) {
        hmnode* curr = iter->current;
        printf("%lx\n", *(long*)(curr->key));
        ll_iterate(iter);
    }
}
