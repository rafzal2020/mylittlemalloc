#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.h"

#define MEMLENGTH 4096
static union {
	char bytes[MEMLENGTH];
	double not_used;
} heap;

int align_bytes(size_t size) {
	if (size % 8 != 0) {
                size = (size % 8) + size;
        }
	return size;
}

void *mymalloc(size_t size, char *file, int line) {
	size = align_bytes(size);	
}

void myfree(void *ptr, char *file, int line) {
	
}
