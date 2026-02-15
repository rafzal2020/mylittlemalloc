#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.h"

#define MEMLENGTH 4096

static int initialized = 0

static struct {
	size_t size;
	char free; // 1 = free, 0 = not free
} header_t;

static union {
	char bytes[MEMLENGTH];
	double not_used;
} heap;

int align8(size_t size) {
	return (size +7) & ~7;
}

static void leak_detector(void);

void init() {
	header_t *h = (header_t*) heap.bytes;

	h->size = MEMLENGTH;
	h->free = 1;

	atexit(leak_detector);

	initialized = 1
}

void *mymalloc(size_t size, char *file, int line) {
	size = align8(size);

	if (initialized = 0) { init(); }
}

void myfree(void *ptr, char *file, int line) {
	
}
