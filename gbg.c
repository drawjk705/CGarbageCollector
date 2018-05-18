// disable ASLR - Address Space Layout Randomization
// echo 0 | sudo tee /proc/sys/kernel/randomize_va_space
// 
// enable ASLR
// echo 2 | sudo tee /proc/sys/kernel/randomize_va_space


#include "gbg.h"

void get_stackframe() {

    uint64_t sp;
    __asm__("mov %%rsp, %0" : "=rm" (sp));

    uint64_t bp;
    __asm__("mov %%rbp, %0" : "=rm" (bp));
    
    sp = bp + (8 * 2);
    bp = *(uint64_t*)bp;

    printf("sp = 0x%lx\n", sp);
    printf("bp = 0x%lx\n", bp);

    uint64_t temp = sp;
    while (temp != bp + 8*3) {

        void* v = (void*) temp;

        // print the value of each 8-byte chunk
        printf("%p - 0x%lx\n", (long*)v, *(long*)v);

        temp+= 8;
    }
}

int addr_comp(void* a, void* b) {
    return *(long*)a - *(long*)b;
}

void* my_malloc(void* value) {

    if (heap_contents == NULL || value == NULL) {
        return NULL;
    }

    void* mallocd = malloc(sizeof(*value));

    long* address = mallocd;

    put(heap_contents, address, mallocd, &addr_comp, NULL);

    return mallocd;
}

void init() {
    heap_contents = create_hashmap();
}

void end() {
    destroy_hashmap(heap_contents);
}

int main() {
    
    long x;

    hashmap* hm = create_hashmap();

    long* ptr = my_malloc(&x);

    print_hm(hm);

    long* ptrb = my_malloc(&x);

    print_hm(hm);

    printf("ptr = %p\n", ptr);
    printf("ptrb = %p\n", ptrb);

    *ptr = 17;
    *ptrb = 133;

    printf("\nhashmap = \n");

    print_hm(hm);

    printf("\n");

    get_stackframe();

    return 0;
}