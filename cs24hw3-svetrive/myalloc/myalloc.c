/*! \file
 * Implementation of a simple memory allocator.  The allocator manages a small
 * pool of memory, provides memory chunks on request, and reintegrates freed
 * memory back into the pool.
 *
 * Adapted from Andre DeHon's CS24 2004, 2006 material.
 * Copyright (C) California Institute of Technology, 2004-2010.
 * All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>

#include "myalloc.h"


/*!
 * These variables are used to specify the size and address of the memory pool
 * that the simple allocator works against.  The memory pool is allocated within
 * init_myalloc(), and then myalloc() and free() work against this pool of
 * memory that mem points to.
 */
int MEMORY_SIZE;
unsigned char *mem;
static unsigned char *freeptr;


/*!
 * This function initializes both the allocator state, and the memory pool.  It
 * must be called before myalloc() or myfree() will work at all.
 *
 * Note that we allocate the entire memory pool using malloc().  This is so we
 * can create different memory-pool sizes for testing.  Obviously, in a real
 * allocator, this memory pool would either be a fixed memory region, or the
 * allocator would request a memory region from the operating system (see the
 * C standard function sbrk(), for example).
 */
void init_myalloc() {

    /*
     * Allocate the entire memory pool, from which our simple allocator will
     * serve allocation requests.
     */
    mem = (unsigned char *) malloc(MEMORY_SIZE);
    if (mem == 0) {
        fprintf(stderr,
                "init_myalloc: could not get %d bytes from the system\n",
		MEMORY_SIZE);
        abort();
    }
    freeptr = mem;
    *((int*) mem) = MEMORY_SIZE - 2*sizeof(int);
    freeptr += MEMORY_SIZE - sizeof(int);
    *((int*) freeptr) = MEMORY_SIZE - 2*sizeof(int);
}


/*!
 * Attempt to allocate a chunk of memory of "size" bytes.  Return 0 if
 * allocation fails.
 * Here, we use best-fit allocation. This method works well if we have a few
 * blocks of various sizes, and doesn't work well if we have many blocks of
 * uniform size.
 * Since we must traverse through each of the memory blocks to find the best
 * block for our allocation, we must access each one and check its size. As
 * a result, our allocation is O(n) in the number of blocks.
 */
unsigned char *myalloc(int size) {
     // points to the header of the first block
     unsigned char * current_block = mem;
     // get size of the first block
     int current_space = *((int *) current_block);
     // set initial best parameters
     unsigned char * best = NULL;
     int best_size = MEMORY_SIZE;

     // check if we've reached the end
     unsigned char * end = current_block + abs(current_space) + 2*sizeof(int);
     while (end <= mem + MEMORY_SIZE) {
       // check if this block fits and if it beats the current best block
       if (current_space >= size && current_space < best_size) {
          best = current_block;
          best_size = current_space;
       }
       // move on to the next block
       else {
         // get current_block to point at the header of the next block
         current_block += abs(current_space) + 2*sizeof(int);
         // current space dereferences the header to get the amount of
         // space available
         current_space =  *((int *) current_block);
       }
       end = current_block + abs(current_space) + 2*sizeof(int);
     }
     // we found a block that is big enough
     if (best) {
       // get a pointer to actual memory block
       unsigned char * resultptr = best + sizeof(int);
       // check to see if we can create another block with the remaining space
       if (best_size - size > 2*sizeof(int)) {
         // set the header of the block to show that it's no longer available
         *((int *) best) = -1 * size;
         // set the footer of the block
         *((int *) (best + sizeof(int) + size)) = -1 * size;
         // set the header of the next block;
         int split_size =  best_size - size - 2*sizeof(int);
         *((int *) (best + 2*sizeof(int) + size)) = split_size;
         // set the footer of the next block
         *((int *) (best + sizeof(int) + best_size)) = split_size;
       }
       else {
         // just set the header and footer of the current block
         *((int *) best) = -1 * best_size;
         *((int *) (best + sizeof(int) + best_size)) = -1 * best_size;
       }
       return resultptr;
     }
     else {
         fprintf(stderr, "myalloc: cannot service request of size %d with"
                 " %d bytes allocated\n", size, current_space);
         return (unsigned char *) 0;
     }
   }

/*!
 * Free a previously allocated pointer.  oldptr should be an address returned by
 * myalloc().
 * Since we're using header and footer tags to store the size of the memory
 * block, we are able to check the memory block to the left and right of the
 * block we are freeing and coalesce. Thus, our deallocation and coalescing of
 * adjacent blocks runs in constant time with respect to the number of memory
 * blocks.
 */
void myfree(unsigned char *oldptr) {
  unsigned char * header = oldptr;
  unsigned char * footer = oldptr;
  unsigned char * right = NULL;
  unsigned char * left = NULL;
  int size = *((int *) (oldptr - sizeof(int)));
  int left_size = 0;
  int right_size = 0;

  // check right block
  if (oldptr + abs(size) +  sizeof(int) < mem + MEMORY_SIZE) {
    // get the header of the right block
    right = oldptr + abs(size) + sizeof(int);
    // get size from the header
    right_size =  *((int *) right);
  }
  // check left block
  if (oldptr - sizeof(int) > mem) {
    // get footer of left block
      left = oldptr - 2*sizeof(int);
      // gets size from the footer
      left_size =  *((int *) left);
  }
  // if both left and right block are free
  if ((left && right) && (left_size > 0 && right_size > 0)) {
    header = oldptr - 2*sizeof(int) - left_size - sizeof(int);
    footer = oldptr + abs(size) + 2*sizeof(int) + right_size;
    size = left_size + 2*sizeof(int) + abs(size) + 2*sizeof(int) + right_size;
  }
  // if just the left block is free
  else if (left && (left_size > 0)) {
    header = oldptr - 2*sizeof(int) - left_size - sizeof(int);
    footer = oldptr + abs(size);
    size = left_size + 2*sizeof(int) + abs(size);
  }
  // if just the right block is free
  else if (right && (right_size > 0)) {
    header = oldptr - sizeof(int);
    footer = oldptr + abs(size) + 2*sizeof(int) + right_size;
    size = right_size + 2*sizeof(int) + abs(size);
  }
  // none of the adjacents blocks are free
  else {
    header = oldptr - sizeof(int);
    footer = oldptr + abs(size);
    size = abs(size);
  }
  // update header and footer
  *((int*) header) = size;
  *((int*) footer) = size;
}

/*!
 * Clean up the allocator state.
 * All this really has to do is free the user memory pool. This function mostly
 * ensures that the test program doesn't leak memory, so it's easy to check
 * if the allocator does.
 */
void close_myalloc() {
    free(mem);
}
