/*
 * General implementation of semaphores.
 *
 *--------------------------------------------------------------------
 * Adapted from code for CS24 by Jason Hickey.
 * Copyright (C) 2003-2010, Caltech.  All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>

#include "sthread.h"
#include "semaphore.h"

/* This data structure is used exclusively with semaphores and allows us to
 * to keep track of a linked list of threads that are blocked by a given
 * semaphore.
 */
typedef struct _blocked_threads {
  Thread * threadp;
  struct _blocked_threads *next;
} BlockedThreads;

/*
 * The semaphore data structure contains a value for the semaphore and two
 * pointers to a linked list of blocked threads (from the head of the list and
 * the tail of the list).
 */
struct _semaphore {
    // Value of the semaphore
    unsigned int value;
    // Pointers to the linked list of threads
    BlockedThreads * head;
    BlockedThreads * tail;
};

/* This function takes in a semaphore and a thread and adds the thread to the
 * linked list of threads that are blocked by the semaphore
 */
void add_thread(Semaphore *s, Thread *t) {
  // We can't perform this operation if s or t are NULL
  assert(s != NULL);
  assert(t != NULL);
  // Allocated memory for the linked list
  BlockedThreads *b = (BlockedThreads *) malloc(sizeof(BlockedThreads));
  if (b == NULL) {
    fprintf(stderr, "Couldn't allocate enough memory for blocked thread linked list");
    exit(2);
  }
  // Set values for the linked list
  b->threadp = t;
  b->next = NULL;
  // The semaphore is not currently blocking any threads
  if (s->head == NULL) {
    // Set both head and tail to be the linked list containing this thread
    assert(s->tail == NULL);
    s->head = b;
    s->tail = b;

  }
  // There are threads that are blocked by this semaphore
  else {
    assert(s->tail != NULL);
    // Add this thread to the end of the list
    s->tail->next = b;
    s->tail = b;
  }
}

/* This function gets a thread that is currently blocked by the semaphore given
 * in the input.
 */
Thread * get_thread(Semaphore *s) {
  // Temporary variable
  Thread *threadp = NULL;
  assert(s != NULL);
  // Check if any threads are blocked by s, if so
  if (s->head != NULL) {
    // Get the first thread off the linked list
    BlockedThreads *b = s->head;
    // Rearrange the linked list to remove this thread
    s->head = b->next;
    if (s->head == NULL) {
      s->tail = NULL;
    }
    // Store this thread to return it
    threadp = b->threadp;

    free(b);
  }
  return threadp;
}

/************************************************************************
 * Top-level semaphore implementation.
 */

/*
 * Allocate a new semaphore.  The initial value of the semaphore is
 * specified by the argument.
 */
Semaphore *new_semaphore(int init) {
    Semaphore *semp = NULL;

    assert(init >= 0);
    // Allocate memory for the new semaphore
    semp = (Semaphore *) malloc(sizeof(Semaphore));
    if (semp == NULL) {
      fprintf(stderr, "Couldn't allocate enough memory for a semaphore");
      exit(2);
    }
    // Set values for the semaphore. Since this is new, there are no threads
    // blocked by it currently.
    semp->value = init;
    semp->head = NULL;
    semp->tail = NULL;

    return semp;
}

/*
 * Decrement the semaphore.
 * This operation must be atomic, and it blocks iff the semaphore is zero.
 */
void semaphore_wait(Semaphore *semp) {
    assert(semp != NULL);
    // Avoid interruption from scheduler
    __sthread_lock();
    assert(semp->value >= 0);
    // Do in a while loop for all threads trying the acquire the semaphore
    while (semp->value == 0) {
      // Block until semaphore is available
      add_thread(semp, sthread_current());
      sthread_block();
      // Block removes the lock, so we have to relock
      __sthread_lock();
    }
    assert(semp->value > 0);
    semp->value = semp->value - 1;
    __sthread_unlock();
}

/*
 * Increment the semaphore.
 * This operation must be atomic.
 */
void semaphore_signal(Semaphore *semp) {
    Thread * t;
    assert(semp != NULL);

    __sthread_lock();
    assert(semp->value >= 0);
    semp->value = semp->value + 1;
    t = get_thread(semp);
    // If there are threads that are blocked by the semaphore, we remove the
    //  first thread from the linked list and unblock it
    if (t != NULL) {
      sthread_unblock(t);
    }
    else {
      __sthread_unlock();
    }
}
