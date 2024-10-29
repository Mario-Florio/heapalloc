#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "heapalloc.h"

#define DEBUGGER

#ifdef DEBUGGER

#define CAPACITY 10
#define PRINTBITMAP(TITLE) \
    printf("%s\n", TITLE); \
    for (int i = 0; i < CAPACITY; i++) { \
        printf("%d ", bitmap[i]); \
    } \
    printf("\n"); \

#else

#define CAPACITY 640000
#define PRINTBITMAP(TITLE) printf("\nDEBUGGER IS UNDEFINED: REMOVE BITMAP PRINTER\n");

#endif

typedef struct {
    void* heap;
    int freedspace;
} Heapdata;

char bitmap[CAPACITY] = {  0 * CAPACITY };

static Heapdata heapdata;

static void heapinit() {
    void* address_space = mmap(NULL, CAPACITY,
                          PROT_READ|PROT_WRITE,
                          MAP_PRIVATE|MAP_ANONYMOUS,
                          0, 0);

    if (address_space == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    heapdata.heap = address_space;
    heapdata.freedspace = CAPACITY;

    PRINTBITMAP("HEAP INIT");
}

void* heapalloc(size_t size) {
    if (heapdata.heap == NULL) heapinit();
    if (size > heapdata.freedspace) {
        perror("Reached max heap capacity");
        exit(1);
    }
    if (size < 1) {
        perror("Must request size greater than 0");
        exit(1);
    }

    int start = 0, freewindow = 0;
    for (int i = 1; i < CAPACITY+1; i++) {
        if (bitmap[i-1] == 0) {
            freewindow++;
        } else {
            freewindow = 0;
            start = i;
        }
    }

    for (int i = start; i < start+size; i++) {
        bitmap[i] = 1;
    }
    bitmap[start] = size;

    void* memaddress = (void*)heapdata.heap+start;
    heapdata.freedspace -= size;

    PRINTBITMAP("HEAP ALLOC");

    return memaddress;
}

void heapfree(void* ptr) {
    
}