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
 * This data structure records pages that are currently loaded in the
 * virtual memory as nodes. This way, we created a linked list allowing
 * us to remove the first page easily.
 */

typedef struct node_t {
    /* Represents one page */
    page_t page;
    /*Points to another page*/
    struct node_t *next;
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
    // Allocate memory for a node for a page
    node_t * n = (node_t *) malloc(sizeof(node_t));
    // Intialize node
    n->page = page;
    n->next = NULL;

    // If there are no nodes in our linked list
    if (head == NULL && tail == NULL) {
      head = n;
      tail = n;
    }
    // Add to the end of our linked list
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

    /* Remove first page */
    node_t * first = head;
    head = head->next;
    free(first);
    first = NULL;


#if VERBOSE
    fprintf(stderr, "Choosing victim page %u to evict.\n", victim);
#endif

    return victim;
}
