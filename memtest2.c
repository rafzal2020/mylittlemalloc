// This test determines if malloc() reserved unallocated memory.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef REALMALLOC
#include "mymalloc.h"
#endif

// Number of objects to allocate
#define NOBJ 32

// Size of each object
#define OBJSIZE 64

int main(void) {
    unsigned char *obj[NOBJ];
    int errors = 0;

    printf("Allocating %d objects of size %d...\n", NOBJ, OBJSIZE);

    // 1. Allocate objects
    for (int i = 0; i < NOBJ; i++) {
        obj[i] = malloc(OBJSIZE);
        if (obj[i] == NULL) {
            printf("Allocation failed for object %d\n", i);
            return 1;
        }
    }

    // 2. Fill each object with a unique byte pattern
    for (int i = 0; i < NOBJ; i++) {
        memset(obj[i], i, OBJSIZE);
    }

    // 3. Verify that each object still contains its pattern
    for (int i = 0; i < NOBJ; i++) {
        for (int j = 0; j < OBJSIZE; j++) {
            if (obj[i][j] != (unsigned char)i) {
                printf("ERROR: Object %d byte %d corrupted (found %d, expected %d)\n",
                       i, j, obj[i][j], i);
                errors++;
                break; // no need to print 64 errors for one block
            }
        }
    }

    // 4. Free all objects
    for (int i = 0; i < NOBJ; i++) {
        free(obj[i]);
    }

    if (errors == 0) {
        printf("SUCCESS: No overlapping allocations detected.\n");
    } else {
        printf("FAILURE: %d objects showed corruption.\n", errors);
    }

    return 0;
}

