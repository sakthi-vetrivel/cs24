/*! \file
 * This file contains code for performing simple tests of the memory allocator.
 * It can be edited by students to perform simple sequences of allocations and
 * deallocations to make sure the allocator works properly.
 */

#include <stdio.h>
#include <stdlib.h>

#include "myalloc.h"


/* Try to allocate a block of memory, and fill its entire contents with
 * the specified fill character.
 */
unsigned char * allocate(int size, unsigned char fill) {
    unsigned char *block = myalloc(size);
    if (block != NULL) {
        int i;

        printf("Allocated block of size %d bytes.\n", size);
        for (i = 0; i < size; i++)
            block[i] = fill;
    }
    else {
        printf("Couldn't allocate block of size %d bytes.\n", size);
    }
    return block;
}


int main(int argc, char *argv[]) {

    /* Specify the memory pool size, then initialize the allocator. */
    MEMORY_SIZE = 100 + 150 + 200 + 300 + 8 * sizeof(int);
    init_myalloc();
    unsigned char *a;
    unsigned char *b;
    unsigned char *c;
    unsigned char *d;

    /* Perform simple allocations and deallocations. */
    /* Change the below code as you see fit, to test various scenarios. */
    a = allocate(100, 'A');
    d = allocate(150, 'D');
    b = allocate(200, 'B');
    c = allocate(300, 'C');

    // case with no coalescing
    myfree(a);
    // case with left
    myfree(d);
    // case with right coalescing
    myfree(c);
    // case with both right and left
    myfree(b);

    close_myalloc();

    return 0;
}
