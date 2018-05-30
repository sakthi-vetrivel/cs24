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

/*
 * The semaphore data structure contains TODO
 */

typedef struct _blocked_threads {
  Thread * threadp;
  struct _blocked_threads *next;
} BlockedThreads;

struct _semaphore {
    unsigned int value;
    BlockedThreads * head;
    BlockedThreads * tail;
};

void add_thread(Semaphore *s, Thread *t) {
  assert(s != NULL);
  assert(t != NULL);

  BlockedThreads *b = (BlockedThreads *) malloc(sizeof(BlockedThreads));
  if (b == NULL) {
    fprintf(stderr, "Couldn't allocate enough memory for blocked thread linked list");
    exit(2);
  }
  b->threadp = t;
  b->next = NULL;
  if (s->head == NULL) {
    assert(s->tail == NULL);
    s->head = b;
    s->tail = b;

  }
  else {
    assert(s->tail != NULL);
    s->tail->next = b;
    s->tail = b;
  }
}

Thread * get_thread(Semaphore *s) {
  Thread *threadp = NULL;
  assert(s != NULL);
  if (s->head != NULL) {
    BlockedThreads * b = s->head;
    s->head = b->next;
    if (s->head == NULL) {
      s->tail = NULL;
    }
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
    semp = (Semaphore *) malloc(sizeof(Semaphore));
    if (semp == NULL) {
      fprintf(stderr, "Couldn't allocate enough memory for a semaphore");
      exit(2);
    }
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
    __sthread_lock();
    assert(semp->value >= 0);

    while (semp->value == 0) {
      add_thread(semp, sthread_current());
      sthread_block();
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
    if (t != NULL) {
      sthread_unblock(t);
    }
    else {
      __sthread_unlock();
    }
}
