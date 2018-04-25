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
 */
unsigned char *myalloc(int size) {

      // points to the header of the first block
     unsigned char * current_block = mem;
     int current_space = *((int *) current_block);
     //unsigned char * best = current_block;

     // if this block isn't big enough
     while (size > current_space) {
       // check to see if we've reached the end of the memory pool
       // header of last block + size of header + size of memory block + size of footer
       // gets us to the header of the next block, want to check if that's MEMORY SIZE
       // moving on, when we increment by current_space, want to make sure that's a positive value
       //printf("%d", current_space);
       current_space = abs(current_space);
       // if we reach the end
       if (current_block + current_space + 2 * sizeof(int) >= mem + MEMORY_SIZE ) {
         current_block = NULL;
         break;
       }
       // want to look at the next block
       else {
         // get current_block to point at the header of the next block
         current_block += abs(current_space) + 2*sizeof(int);
         // current space dereferences the header to get the amount of space available
         current_space =  *((int *) current_block);
       }
     }
     // we have a block that is a decent size
     if (current_block) {
       // get a pointer to actual memory block
       unsigned char * resultptr = current_block + sizeof(int);
       // check to see if we can create another block with the remaining space
       if (current_space - size > 2*sizeof(int)) {
         // set the header of the block to show that it's no longer available
         *((int *) current_block) = -1 * size;
         // set the footer of the block
         *((int *) (current_block + sizeof(int) + size)) = -1 * size;
         // set the header of the next block;
         *((int *) (current_block + 2*sizeof(int) + size)) = current_space - size - 2*sizeof(int);
         // set the footer of the next block
         *((int *) (current_block + sizeof(int) + current_space)) = current_space - size - 2*sizeof(int);
       }
       else {
         // just set the header and footer of the current block
         *((int *) current_block) = -1 * current_space;
         *((int *) (current_block + sizeof(int) + current_space)) = -1 * current_space;
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
 */
void myfree(unsigned char *oldptr) {
  // set that block to be freed
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
  else if (left && (left_size > 0)) {
    header = oldptr - 2*sizeof(int) - left_size - sizeof(int);
    footer = oldptr + abs(size);
    size = left_size + 2*sizeof(int) + abs(size);
  }
  else if (right && (right_size > 0)) {
    header = oldptr - sizeof(int);
    footer = oldptr + abs(size) + 2*sizeof(int) + right_size;
    size = right_size + 2*sizeof(int) + abs(size);
  }
  else {
    header = oldptr - sizeof(int);
    footer = oldptr + abs(size);
    size = abs(size);
  }
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
