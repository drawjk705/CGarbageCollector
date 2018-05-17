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

void* my_malloc()


int main() {
    
    long x = 5;
    long y = 6;
    long z = 7;

    long* p = &x;

    get_stackframe();

    return 0;
}