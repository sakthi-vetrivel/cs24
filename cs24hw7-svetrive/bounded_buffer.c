/*
 * Define a bounded buffer containing records that describe the
 * results in a producer thread.
 *
 *--------------------------------------------------------------------
 * Adapted from code for CS24 by Jason Hickey.
 * Copyright (C) 2003-2010, Caltech.  All rights reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>

#include "sthread.h"
#include "semaphore.h"
#include "bounded_buffer.h"

/*
 * The bounded buffer data.
 */
struct _bounded_buffer {
    /* The maximum number of elements in the buffer */
    int length;

    /* The index of the first element in the buffer */
    int first;

    /* The number of elements currently stored in the buffer */
    int count;

    /* The values in the buffer */
    BufferElem *buffer;

    Semaphore * has_space;

    Semaphore * has_data;

    Semaphore * access;

};


#define EMPTY -1


/*
 * Allocate a new bounded buffer.
 */
BoundedBuffer *new_bounded_buffer(int length) {
    BoundedBuffer *bufp;
    BufferElem *buffer;
    int i;

    /* Allocate the buffer */
    buffer = (BufferElem *) malloc(length * sizeof(BufferElem));
    bufp = (BoundedBuffer *) malloc(sizeof(BoundedBuffer));
    if (buffer == 0 || bufp == 0) {
        fprintf(stderr, "new_bounded_buffer: out of memory\n");
        exit(1);
    }

    /* Initialize */

    memset(bufp, 0, sizeof(BoundedBuffer));

    for (i = 0; i != length; i++) {
        buffer[i].id = EMPTY;
        buffer[i].arg = EMPTY;
        buffer[i].val = EMPTY;
    }

    bufp->length = length;
    bufp->buffer = buffer;

    bufp->has_space = new_semaphore(length);
    bufp->has_data = new_semaphore(0);
    bufp->access = new_semaphore(1);

    return bufp;
}

/*
 * Add an integer to the buffer.  Yield control to another
 * thread if the buffer is full.
 */
void bounded_buffer_add(BoundedBuffer *bufp, const BufferElem *elem) {

    semaphore_wait(bufp->has_space);
    semaphore_wait(bufp->access);
    /* Wait until the buffer has space */
    bufp->buffer[(bufp->first + bufp->count) % bufp->length] = *elem;

    bufp->count = bufp->count + 1;

    semaphore_signal(bufp->access);
    semaphore_signal(bufp->has_data);
}

/*
 * Get an integer from the buffer.  Yield control to another
 * thread if the buffer is empty.
 */
void bounded_buffer_take(BoundedBuffer *bufp, BufferElem *elem) {
  semaphore_wait(bufp->has_data);
  semaphore_wait(bufp->access);
  /* Copy the element from the buffer, and clear the record */
  *elem = bufp->buffer[bufp->first];
  bufp->buffer[bufp->first].id = EMPTY;
  bufp->buffer[bufp->first].arg = EMPTY;
  bufp->buffer[bufp->first].val = EMPTY;

  bufp->count = bufp->count - 1;
  bufp->first = (bufp->first + 1) % bufp->length;

  semaphore_signal(bufp->access);
  semaphore_signal(bufp->has_space);
}
