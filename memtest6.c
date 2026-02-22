#include <stdio.h>
#include <stdlib.h>

#include "mymalloc.h"

int main(void) {
    printf("Test 6: leak detection\n");

    void *a = malloc(200);
    void *b = malloc(300);

    // Intentionally leak a and b
    // Program ends without freeing them

    return 0;
}

