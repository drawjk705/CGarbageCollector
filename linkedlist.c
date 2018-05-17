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

void destroy_node(hmnode* n, int free_val) {
	if (free_val) {
		free(n->key);
		free(n->value);
	}
	free(n);
}

linkedlist* create_ll() {
         linkedlist* ll = malloc(sizeof(linkedlist));
         return ll;
}

void add_to_head(linkedlist* ll, void* key, void* value) {

        if (ll == NULL || key == NULL || value == NULL) {
              return;
        }
        hmnode* n = create_node(key, value);
        if (n == NULL) {
                return;
        }
        ll->head = n;
}

void add_to_tail(linkedlist* ll, void* key, void* value) {
	
	if (ll == NULL || key == NULL || value == NULL) {
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

	if (ll == NULL || key == NULL) {
		return 0;
	}

	hmnode* trav = ll->head;

	while (trav != NULL) {
		if (compare(trav->key, key)) {
			return 1;
		}
		trav = trav->next;
	}

	return 0;
}

void ll_remove(linkedlist* ll, void* key, int (*compare)(void*, void*)) {

	if (ll == NULL || key == NULL) {
		return;
	}

	hmnode* trav = ll->head;
	hmnode* prev;

	while (trav != NULL) {
		if (compare(trav->key, key)) {
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
	hmnode* temp;

	while (trav->next != NULL) {
		temp = trav->next;
		destroy_node(trav, free_val);
		trav = temp;
	}
	free(ll);
}








