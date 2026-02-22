// This test determines if free() deallocates memory properly.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mymalloc.h"

int main(void) {
    printf("Test 3: free() actually deallocates memory\n");

    char *p = malloc(100);
    if (!p) { printf("malloc failed\n"); return 1; }

    memset(p, 0xAA, 100);
    free(p);

    char *q = malloc(100);
    if (!q) { printf("malloc failed\n"); return 1; }

    if (q == p) {
        printf("SUCCESS: allocator reused freed block\n");
    } else {
        printf("WARNING: allocator did not reuse freed block\n");
    }

    free(q);
    return 0;
}

