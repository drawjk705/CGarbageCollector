// disable ASLR - Address Space Layout Randomization
// echo 0 | sudo tee /proc/sys/kernel/randomize_va_space
// 
// enable ASLR
// echo 2 | sudo tee /proc/sys/kernel/randomize_va_space


#include "gbg.h"

/**
 * gets stackframe for function that is n
 * frames beack
 * @param  frame_backup number of frames to look back
 * @return              linkedlist* of the stackframe
 */
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

    // print stack and base pointer
    printf("sp = 0x%lx\n", sp);
    printf("bp = 0x%lx\n", bp);
    
    // create linkedlist
    linkedlist* stack_contents = create_ll();

    // store "address" of stack pointer
    uint64_t temp = sp;

    while (temp != bp + 8 * 3) {

        // create a pointer from the address
        void* v = (void*) temp;

        // print the value of each 8-byte chunk
        printf("%p - 0x%lx\n", (long*)v, *(long*)v);

        // store potential address...
        long* addr = malloc(sizeof(long));
        *addr = *(long*)v;
    
        // ...and add to linkedlist
        add_to_tail(stack_contents, addr, NULL);

        // increment temporary stack address
        temp+= 8;
    }

    return stack_contents;
}

/**
 * function to assert equality
 * between 2 addresses
 * @param  a first address
 * @param  b second address
 * @return   0 if equal, anything else if not
 */
int addr_comp(void* a, void* b) {
    return *(long*)a - *(long*)b;
}

/**
 * custom malloc() function that will store
 * all malloc()'d memory in a global hashmap
 * @param size size_t of the data type to malloc()
 */
void* my_malloc(size_t size) {

    // error check
    if (heap_contents == NULL || size == 0) {
        return NULL;
    }

    // malloc()
    void* mallocd = malloc(size);

    // store address of malloc()'d pointer
    long* address = malloc(sizeof(long));
    char addr[20];
    sprintf(addr, "%p", mallocd);
    *address = hex_to_long(addr);

    // store metadata about pointer
    meta* md = malloc(sizeof(meta));
    md->size = size;
    md->marked = 0;
    md->ptr = mallocd;

    // add to hashmap
    put(heap_contents, address, md, &addr_comp, &addr_hash);

    return mallocd;
}

/**
 * hash function for an address
 * @param  addr pointer to the address
 * @return      hash code
 */
long addr_hash(void* addr) {
    return *(long*)addr;
}

/**
 * assesses whether or not an address
 * is a pointer, by checking for membership
 * in the heap_contents hashmap
 * @param  val address
 * @return     1 if true, 0 if false
 */
int is_ptr(void* val) {
    return contains_key(heap_contents, val, &addr_comp, &addr_hash);
}

void mark_contents(void* addr) {

    // get ptr metadata from heap_contents
    meta* md = (meta*)get_from_hm(heap_contents, addr, &addr_comp, &addr_hash);

    // base case
    if (md->marked == 1) {
        return;
    }

    // mark
    md->marked = 1;

    // get size
    int size = md->size;

    // ensure size is a multiple of 8
    while (size % 8 != 0) {
        size++;
    }

    // get the address
    long temp = *(long*)addr;

    printf("\n\n\n");

    // DFS for accessing malloc()'d spaces
    // that aren't on the stack

    // go through each value contained by ptr
    for (int i = 0; i < size; i += 8) {
        printf("0x%lx: 0x%lx\n", temp + i, *(long*)(void*)(temp + i));

        // if value is a pointer, mark it
        if (is_ptr((void*)(temp + i))) {
            mark_contents((void*)(temp + i));
        }
    }
}

/**
 * go through heap_contents and pre-mark all values
 * with 0
 */
void pre_mark() {

    // iterate and mark
    hmiter* iter = create_iter(heap_contents);

    while (iter->kvp != NULL) {

        hmnode* kvp = iter->kvp;

        meta* md = (meta*)kvp->value;

        md->marked = 0;

        iterate(iter);
    }

    free(iter);
}

/**
 * free() all unmarked values on the heap
 */
void discard_unmarked() {

    // iterate through heap_contents
    hmiter* iter = create_iter(heap_contents);

    while (iter->kvp != NULL) {

        hmnode* kvp = iter->kvp;

        meta* md = (meta*) kvp->value;

        // discard if are not marked
        if (md->marked == 0) {

            printf("discarding %p\n", md->ptr);

            free(md->ptr);

            remove_from_hm(heap_contents, kvp->key, &addr_comp, &addr_hash);

        }

        iterate(iter);
    }
    free(iter);
}

/**
 * keep all marked values,
 * and free() all unmarked
 */
void collect_garbage() {

    // pre-mark everything with 0
    pre_mark();
  
    // get this function's caller's stackframe
    linkedlist* stackframe = get_stackframe(2);

    // iterate through stackframe contents
    lliter* iter = create_lliter(stackframe);

    while (iter->current != NULL) {

        hmnode* temp = (hmnode*)iter->current;

        long* key = (long*)temp->key;

        // if the stackframe value is a pointer,
        // mark it
        if (is_ptr(key)) {
            mark_contents(key);
        }

        ll_iterate(iter);
    }

    // discard all unmarked values
    discard_unmarked();

    // free() everything
    free(iter);
    destroy_list(stackframe, 0);
}

/**
 * initialize heap_contents hashmap
 */
void init() {
    heap_contents = create_hashmap();
}

// clean up
void end() {
    destroy_hashmap(heap_contents);
}

/**
 * function to test nested malloc()'s
 * @param ll [description]
 */
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

/**
 * driver
 */
int main() {

    // initialize heap_contents hashmap
    init();

    // do some malloc()-ing
    linkedlist* ll = my_malloc(sizeof(linkedlist));
    fun(ll);

    printf("\nheap contents = \n");

    print_hm(heap_contents);

    printf("\ncollecting garbage\n");

    collect_garbage();

    printf("\nheap contents = \n");

    print_hm(heap_contents);

    end();

    return 0;
}