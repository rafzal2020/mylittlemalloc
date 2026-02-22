#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#include "mymalloc.h"

#define RUNS 50
#define OPS 120

// Helper: compute time difference in microseconds
long elapsed_us(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) * 1000000L +
           (end.tv_usec - start.tv_usec);
}

int main(void) {
    struct timeval start, end;
    long total_time;
    int i, j;

    printf("=== memgrind ===\n");

    // -------------------------------
    // WORKLOAD A
    // malloc() then free() a 1-byte object, 120 times
    // -------------------------------
    total_time = 0;
    for (i = 0; i < RUNS; i++) {
        gettimeofday(&start, NULL);

        for (j = 0; j < OPS; j++) {
            char *p = malloc(1);
            free(p);
        }

        gettimeofday(&end, NULL);
        total_time += elapsed_us(start, end);
    }
    printf("Workload A average time: %ld us\n", total_time / RUNS);

    // -------------------------------
    // WORKLOAD B
    // malloc() 120 1-byte objects, store pointers, then free them
    // -------------------------------
    total_time = 0;
    for (i = 0; i < RUNS; i++) {
        char *arr[OPS];

        gettimeofday(&start, NULL);

        for (j = 0; j < OPS; j++) {
            arr[j] = malloc(1);
        }
        for (j = 0; j < OPS; j++) {
            free(arr[j]);
        }

        gettimeofday(&end, NULL);
        total_time += elapsed_us(start, end);
    }
    printf("Workload B average time: %ld us\n", total_time / RUNS);

    // -------------------------------
    // WORKLOAD C
    // Randomized allocate/free pattern
    // -------------------------------
    total_time = 0;
    for (i = 0; i < RUNS; i++) {
        char *arr[OPS];
        int allocated = 0;

        // initialize array
        for (j = 0; j < OPS; j++) arr[j] = NULL;

        gettimeofday(&start, NULL);

        int ops_done = 0;
        while (ops_done < OPS) {
            int coin = rand() % 2;

            if (coin == 0) {
                // allocate if we haven't reached 120 allocations
                arr[allocated] = malloc(1);
                allocated++;
                ops_done++;
            } else {
                // free a random allocated object (if any)
                if (allocated > 0) {
                    int idx = rand() % allocated;
                    free(arr[idx]);

                    // move last allocated pointer into freed slot
                    arr[idx] = arr[allocated - 1];
                    allocated--;
                }
            }
        }

        // free remaining allocated objects
        for (j = 0; j < allocated; j++) {
            free(arr[j]);
        }

        gettimeofday(&end, NULL);
        total_time += elapsed_us(start, end);
    }
    printf("Workload C average time: %ld us\n", total_time / RUNS);

    // -------------------------------
    // WORKLOAD D
    // Build and destroy a linked list of 120 nodes
    // -------------------------------
    total_time = 0;
    for (i = 0; i < RUNS; i++) {

        gettimeofday(&start, NULL);

        typedef struct Node {
            char data[16];
            struct Node *next;
        } Node;

        Node *head = NULL;

        // Build list
        for (j = 0; j < OPS; j++) {
            Node *n = malloc(sizeof(Node));
            n->data[0] = (char)j;   // touch memory
            n->next = head;
            head = n;
        }

        // Free list
        Node *curr = head;
        while (curr != NULL) {
            Node *next = curr->next;
            free(curr);
            curr = next;
        }

        gettimeofday(&end, NULL);
        total_time += elapsed_us(start, end);
    }

    printf("Workload D average time: %ld us\n", total_time / RUNS);
    

    // -------------------------------
    // WORKLOAD E
    // Build a binary tree of ~120 nodes and free it post-order
    // -------------------------------
    total_time = 0;
    for (i = 0; i < RUNS; i++) {

        gettimeofday(&start, NULL);

        typedef struct Tree {
            char payload[24];
            struct Tree *left;
            struct Tree *right;
        } Tree;

        // Simple recursive tree builder
        Tree* build(int depth) {
            if (depth == 0) return NULL;
            Tree *node = malloc(sizeof(Tree));
            node->payload[0] = (char)depth; // touch memory
            node->left  = build(depth - 1);
            node->right = build(depth - 1);
            return node;
        }

        // Post-order free
        void freeTree(Tree *t) {
            if (!t) return;
            freeTree(t->left);
            freeTree(t->right);
            free(t);
        }

        Tree *root = build(6);  // roughly 127 nodes
        freeTree(root);

        gettimeofday(&end, NULL);
        total_time += elapsed_us(start, end);
    }

    printf("Workload E average time: %ld us\n", total_time / RUNS);

}

