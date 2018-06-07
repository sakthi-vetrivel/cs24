/*============================================================================
 * Implementation of the FIFO page replacement policy.
 *
 * We don't mind if paging policies use malloc() and free(), just because it
 * keeps things simpler.  In real life, the pager would use the kernel memory
 * allocator to manage data structures, and it would endeavor to allocate and
 * release as little memory as possible to avoid making the kernel slow and
 * prone to memory fragmentation issues.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "vmpolicy.h"


/*============================================================================
 * "Node" Data Structure
 *
 * This data structure records all pages that are currently loaded in the
 * virtual memory, so that we can choose a random page to evict very easily.
 */

typedef struct node_t {
    /* The maximum number of pages that can be resident in memory at once. */
    page_t page;

    /* The number of pages that are currently loaded.  This can initially be
     * less than max_resident.
     */
    struct node_t *next;

    /* This is the array of pages that are actually loaded.  Note that only the
     * first "num_loaded" entries are actually valid.
     */
} node_t;


/*============================================================================
 * Policy Implementation
 */


/* The list of pages that are currently resident. */
static node_t *head;
static node_t *tail;


/* Initialize the policy.  Return nonzero for success, 0 for failure. */
int policy_init(int max_resident) {
    fprintf(stderr, "Using FIFO eviction policy.\n\n");

    head = NULL;
    tail = NULL;

    /* Return nonzero if initialization succeeded. */
    return 1;
}


/* Clean up the data used by the page replacement policy. */
void policy_cleanup(void) {
    head = NULL;
    tail = NULL;
}


/* This function is called when the virtual memory system maps a page into the
 * virtual address space.  Record that the page is now resident.
 */
void policy_page_mapped(page_t page) {
    node_t * n = (node_t *) malloc(sizeof(node_t));
    n->page = page;
    n->next = NULL;

    if (head == NULL && tail == NULL) {
      head = n;
      tail = n;
    }
    else {
      tail->next = n;
      tail = n;
    }
    return;
}


/* This function is called when the virtual memory system has a timer tick. */
void policy_timer_tick(void) {
    /* Do nothing! */
}


/* Choose a page to evict from the collection of mapped pages.  Then, record
 * that it is evicted.  This is very simple since we are implementing a random
 * page-replacement policy.
 */
page_t choose_and_evict_victim_page(void) {
    page_t victim = head->page;

    /* Figure out which page to evict. */
    node_t * first = head;
    head = head->next;
    free(first);
    first = NULL;


#if VERBOSE
    fprintf(stderr, "Choosing victim page %u to evict.\n", victim);
#endif

    return victim;
}
