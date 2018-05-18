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

int long_comp(void* a, void* b) {
    return *(long*)a - *(long*)b;
}

void* my_malloc(void* value, hashmap* heap_contents) {

    if (heap_contents == NULL || value == NULL) {
        return NULL;
    }

    void* mallocd = malloc(sizeof(*value));

    long* address = mallocd;

    put(heap_contents, address, mallocd, &long_comp, NULL);

    return mallocd;
}

int main() {
    
    


    
    return 0;
}