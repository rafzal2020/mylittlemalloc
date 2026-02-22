// Testing section 2 edge cases.

#include <stdio.h>
#include <stdlib.h>

#include "mymalloc.h"

int main(void) {
    printf("Test 5: error detection tests\n");

    // 1. Freeing a pointer not from malloc
    int x;
    free(&x);   // should print error and exit(2)

    // 2. Freeing a pointer not at start of chunk
    int *p = malloc(sizeof(int) * 4);
    free(p + 1);   // should print error and exit(2)

    // 3. Double free
    int *q = malloc(50);
    free(q);
    free(q);       // should print error and exit(2)

    return 0;
}

