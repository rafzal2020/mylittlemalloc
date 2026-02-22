// This tests for coalescing adjacent free blocks

#include <stdio.h>
#include <stdlib.h>

#include "mymalloc.h"

int main(void) {
    printf("Test 4: coalescing adjacent free blocks\n");

    void *a = malloc(100);
    void *b = malloc(100);
    void *c = malloc(100);

    free(b);
    free(a);   // a and b should coalesce here

    // Now request something larger than 100 but smaller than 200
    void *x = malloc(150);

    if (x == a) {
        printf("SUCCESS: coalescing works\n");
    } else {
        printf("FAILURE: coalescing did not occur\n");
    }

    free(c);
    free(x);
    return 0;
}

