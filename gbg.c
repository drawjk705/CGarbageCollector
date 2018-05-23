// disable ASLR - Address Space Layout Randomization
// echo 0 | sudo tee /proc/sys/kernel/randomize_va_space
// 
// enable ASLR
// echo 2 | sudo tee /proc/sys/kernel/randomize_va_space


#include "gbg.h"

linkedlist* get_stackframe(int frame_backup) {

    uint64_t sp;
    __asm__("mov %%rsp, %0" : "=rm" (sp));

    uint64_t bp;
    __asm__("mov %%rbp, %0" : "=rm" (bp));
    
    // move back n frames
    for (int i = 0; i < frame_backup; i++) {
        sp = bp + (8 * 2);
        bp = *(uint64_t*)bp;
    }

    printf("sp = 0x%lx\n", sp);
    printf("bp = 0x%lx\n", bp);

    printf("%lu\n", bp - sp);
    
    linkedlist* stack_contents = create_ll();

    uint64_t temp = sp;

    while (temp != bp + 8 * 3) {

        void* v = (void*) temp;

        // print the value of each 8-byte chunk
        printf("%p - 0x%lx\n", (long*)v, *(long*)v);

        long* addr = malloc(sizeof(long));
        *addr = *(long*)v;
    
        add_to_tail(stack_contents, addr, NULL);

        temp+= 8;
    }

    return stack_contents;
}

int addr_comp(void* a, void* b) {
    return *(long*)a - *(long*)b;
}

void* my_malloc(size_t size) {

    if (heap_contents == NULL || size == 0) {
        return NULL;
    }

    // printf("%lu\n", sizeof(size));

    void* mallocd = malloc(size);

    long* address = malloc(sizeof(long));

    char addr[20];

    sprintf(addr, "%p", mallocd);

    *address = hex_to_long(addr);

    meta* md = malloc(sizeof(meta));

    md->size = sizeof(size);
    md->marked = 0;
    md->ptr = mallocd;

    put(heap_contents, address, md, &addr_comp, NULL);

    return mallocd;
}

long addr_hash(void* addr) {
    return *(long*)addr;
}


int is_ptr(void* val) {
    return contains_key(heap_contents, val, &addr_comp, &addr_hash);
}

void mark_contents(void* ptr) {

    // get ptr metadata from heap_contents
    
    meta* md = (meta*)get_from_hm(heap_contents, ptr, &addr_comp, NULL);

    // base case
    if (md->marked == 1) {
        return;
    }

    // mark
    md->marked = 1;

    printf("marked %p\n", md->ptr);

    // get size
    int size = md->size;

    // ensure size is a multiple of 8
    while (size % 8 != 0) {
        size++;
    }

    long* temp = malloc(sizeof(long));
    *temp = *(long*)ptr;

    // go through each value contained by ptr
    while (*temp < *(long*)ptr + size) {

        // if the value is a ptr...
        if (is_ptr(temp)) {
            // mark that ptr's contents
            mark_contents(temp);
        }

        // increment temp
        *temp+= 8;
    }
    free(temp);

}

void pre_mark() {

    hmiter* iter = create_iter(heap_contents);

    while (iter->kvp != NULL) {

        hmnode* kvp = iter->kvp;

        meta* md = (meta*)kvp->value;

        md->marked = 0;

        iterate(iter);
    }

    free(iter);
}

void discard_unmarked() {

    hmiter* iter = create_iter(heap_contents);

    while (iter->kvp != NULL) {

        hmnode* kvp = iter->kvp;

        meta* md = (meta*) kvp->value;

        if (md->marked == 0) {

            printf("discarding %p\n", md->ptr);

            free(md->ptr);

            remove_from_hm(heap_contents, kvp->key, &addr_comp, &addr_hash);

        }

        iterate(iter);
    }
    free(iter);
}


void collect_garbage() {

    // pre-mark everything with 0
    pre_mark();
  
    // get this function's caller's stackframe
    linkedlist* stackframe = get_stackframe(2);

    // printf("\n\n\n");

    // print_ll(stackframe);

    // printf("\n\n\n");

    lliter* iter = create_lliter(stackframe);


    while (iter->current != NULL) {

        hmnode* temp = (hmnode*)iter->current;

        long* key = (long*)temp->key;

        printf("%lx\n", *key);

        // set everything in the stackframe that's been malloc'd to 1
        if (is_ptr(key)) {
            mark_contents(key);
        }

        ll_iterate(iter);
    }

    discard_unmarked();

    free(iter);
    destroy_list(stackframe, 0);
}

void init() {
    heap_contents = create_hashmap();
}

void end() {
    destroy_hashmap(heap_contents);
}

void fun(linkedlist* ll) {

    // create node
    hmnode* n = my_malloc(sizeof(hmnode));

    n->key = my_malloc(sizeof(int));
    *(int*)n->key = 3;
    n->value = my_malloc(sizeof(int));
    *(int*)n->value = 4;
    n->next = NULL;


    if (ll->head == NULL) {
        ll->head = n;
    } else {
        n->next = ll->head;
        ll->head = n;
    }

}

int main() {

    init();


    linkedlist* ll = my_malloc(sizeof(linkedlist));

    fun(ll);


    printf("\nheap contents = \n");

    print_hm(heap_contents);

    // printf("\n");
    
    // printf("initial stackframe\n");

    // get_stackframe(1);

    // printf("\n");

    printf("collecting garbage\n");

    collect_garbage();

    // printf("\n");

    // printf("new stackframe\n");

    // get_stackframe(1);

    printf("\nheap contents = \n");

    print_hm(heap_contents);

    return 0;
}