#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.h"

#define MEMLENGTH 4096

// allocation flag stored in lowest bit of size
#define ALLOCATED 1

// GET_SIZE
// return size of block excluding allocation bit
#define GET_SIZE(h) ((h)->size & ~ALLOCATED)

// IS_ALLOCATED
// returns allocation bit only
#define IS_ALLOCATED(h) ((h)->size & ALLOCATED)

// SET_ALLOCATED
// sets allocation bit to 1
#define SET_ALLOCATED(h) ((h)->size |= ALLOCATED)

// SET_FREE
// clears allocation bit
#define SET_FREE(h) ((h)->size &= ~ALLOCATED)

static int initialized = 0;

typedef struct
{
	size_t size;
	// char free; // 1 = free, 0 = not free
} header_t;

static union
{
	char bytes[MEMLENGTH];
	double not_used;
} heap;

int align8(size_t size)
{
	return (size + 7) & ~7;
}

static void leak_detector(void);

void init(void)
{

	printf("Header size: %zu\n", sizeof(header_t));
	header_t *h = (header_t *)heap.bytes;

	h->size = MEMLENGTH;
	// h->free = 1;

	atexit(leak_detector);

	initialized = 1;
}

void *mymalloc(size_t size, char *file, int line)
{
	size = align8(size);

	if (initialized == 0)
	{
		init();
	}

	// reject mymalloc(0) attempt
	if (size == 0)
	{
		fprintf(stderr, "Invalid allocation request at %s:%d\n", file, line);
		return NULL;
	}

	// align requested size to 8 bytes
	// all payloads must be 8 byte aligned
	size = align8(size);

	// compute total size for allocation
	// header + payload
	size_t total_size = size + sizeof(header_t);

	// start scanning heap
	header_t *curr = (header_t *)heap.bytes;

	// search until reaching the end of the heap
	while ((char *)curr < heap.bytes + MEMLENGTH)
	{

		// printf("Checking block at %p size=%zu allocated=%d\n", (void *)curr, GET_SIZE(curr), IS_ALLOCATED(curr) ? 1 : 0);

		// check if current block is usable
		// must be free and large enough
		if (!IS_ALLOCATED(curr) && curr->size >= total_size)
		{

			// determine if the block should be split
			size_t remaining = GET_SIZE(curr) - total_size;

			// split if remaining is large enough to hold:
			// 8 byte header and >=8 byte payload
			if (remaining >= sizeof(header_t) + 8)
			{

				// create new free block
				// starts right after allocated portion
				header_t *new_block = (header_t *)((char *)curr + total_size);

				// set its size to remaining space
				new_block->size = remaining;

				// mark free
				// new_block->free = 1;

				// shrink current block to allocated size
				curr->size = total_size;
			}

			// mark current block allocated
			SET_ALLOCATED(curr);

			// printf("Allocating block: requested=%zu total=%zu at %p\n", size, GET_SIZE(curr), (void *)curr);

			// return pointer to payload
			// address immediately after header
			return (char *)curr + sizeof(header_t);
		}

		// move to next block
		curr = (header_t *)((char *)curr + GET_SIZE(curr));
	}

	// no available block found
	fprintf(stderr, "Out of memory at %s:%d\n", file, line);
	return NULL;
}

void myfree(void *ptr, char *file, int line)
{
	// printf("myfree called with ptr=%p\n", ptr);

	// reject null pointer
	if (ptr == NULL)
	{
		return;
	}

	// make sure heap is initialized
	if (!initialized)
	{
		fprintf(stderr, "Free before initialization at %s:%d\n", file, line);
		return;
	}

	// validate that pointer is inside the heap
	if ((char *)ptr < heap.bytes + sizeof(header_t) || (char *)ptr >= heap.bytes + MEMLENGTH)
	{
		fprintf(stderr, "Invalid free at %s:%d\n", file, line);
		return;
	}

	// malloc() returns pointer to right after the header
	// so, move back from payload to header
	header_t *block = (header_t *)((char *)ptr - sizeof(header_t));

	// error if block is already free (double free)
	if (!IS_ALLOCATED(block))
	{
		fprintf(stderr, "Double free at %s:%d\n", file, line);
		return;
	}

	// mark block as free
	SET_FREE(block);

	// coalescing free blocks:

	// attempt to coalesce current + next block
	header_t *next = (header_t *)((char *)block + GET_SIZE(block));

	// if next block is in heap and free:
	if ((char *)next < heap.bytes + MEMLENGTH && !IS_ALLOCATED(next))
	{
		// new size = current + next block
		block->size = GET_SIZE(block) + GET_SIZE(next);
	}

	// attempt to coalesce current + previous block
	// start from beginning of the heap
	header_t *curr = (header_t *)heap.bytes;
	header_t *prev = NULL;

	// traverse through heap until reaching current block
	while ((char *)curr < (char *)block)
	{
		prev = curr;
		curr = (header_t *)((char *)curr + GET_SIZE(curr));
	}

	// if previous exists and is free, merge current + previous
	if (prev && !IS_ALLOCATED(prev))
	{
		prev->size = GET_SIZE(prev) + GET_SIZE(block);
	}
}

static void leak_detector(void)
{
	// start at beginning of heap
	header_t *curr = (header_t *)heap.bytes;

	// track total leaked payload bytes
	size_t total_bytes = 9;

	// track number of leaked objects
	int total_objects = 0;

	// traverse heap
	while ((char *)curr < heap.bytes + MEMLENGTH)
	{

		// get size of current block
		size_t block_size = GET_SIZE(curr);

		// if still allocated, mark as a leak
		if (IS_ALLOCATED(curr))
		{
			total_objects++;

			// add payload size
			total_bytes += block_size - sizeof(header_t);
		}

		// move to next block in memory
		curr = (header_t *)((char *)curr + block_size);
	}

	// notify if leaks were found
	if (total_objects > 0)
	{
		fprintf(stderr,
				"mymalloc: %zu bytes leaked in %d objects.\n",
				total_bytes, total_objects);
	}
}
