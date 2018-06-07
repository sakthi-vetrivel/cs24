/*============================================================================
 * Implementation of the CLOCK/LRU page replacement policy.
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
 * This data structure is one node in a queue. It stores a page and the next
 * node in the queue.
 */
typedef struct node_t {
    page_t page;
    struct node_t *next;
} node_t;


/*============================================================================
 * "Queue" Data Structure
 *
 * This data structure records all pages that are currently loaded in the
 * virtual memory. It stores all of these pages in a queue.
 */
typedef struct queue_t {
    node_t *head;
    node_t *tail;
} queue_t;


/*============================================================================
 * Policy Implementation
 */


/* The list of pages that are currently resident. */
static queue_t loaded;


/* Initialize the policy.  Return nonzero for success, 0 for failure. */
int policy_init(int max_resident) {
    fprintf(stderr, "Using CLOCK/LRU eviction policy.\n\n");

    /* Initialize queue */
    loaded.head = NULL;
    loaded.tail = NULL;

    /* Return nonzero if initialization succeeded. */
    return 1;
}


/* Clean up the data used by the page replacement policy. */
void policy_cleanup(void) {
    /* No memory to deallocate */
}

/* This function is called when the virtual memory system maps a page into the
 * virtual address space.  Record that the page is now resident.
 */
void policy_page_mapped(page_t page) {
    /* Allocate space for a new node in the loaded pages queue */
    node_t *new = (node_t *) malloc(sizeof(node_t));

    /* Initialize the new node and add it to the end of the queue */
    new->page = page;
    new->next = NULL;

    /* Handle the case where the queue is empty */
    if (loaded.head == NULL && loaded.tail == NULL) {
        loaded.head = new;
    }
    /* Otherwise simply add the node to the end of the queue */
    else {
        loaded.tail->next = new;
    }

    loaded.tail = new;
}


/* This function is called when the virtual memory system has a timer tick. */
void policy_timer_tick(void) {
    node_t *curr = loaded.head;
    node_t *prev = NULL;
    node_t *old_tail = loaded.tail;
    node_t *next;
    /* Traverse the queue of loaded pages until we reach the original tail */
    while (prev != old_tail) {
        /* If a page has been accessed since the last timer interrupt */
        if (is_page_accessed(curr->page)) {
            /* Clear the page's accessed bit */
            clear_page_accessed(curr->page);

            /* Set its permissions as per part (a) */
            set_page_permission(curr->page, PAGEPERM_NONE);

            /* Move the page to the back of the queue if it isn't already
             * at the end of the queue.
             */
            if (curr->next) {
                if (prev) {
                    /* If we are not at the front of the queue, we simply
                     * update prev->next
                     */
                    prev->next = curr->next;
                }
                else {
                    /* If we are at the front of the queue, we need to set
                     * a new head for the queue
                     */
                    loaded.head = curr->next;
                }

                /* Update curr, prev, and the head and tail of the queue */
                next = curr->next;
                curr->next = NULL;
                loaded.tail->next = curr;
                loaded.tail = curr;
                curr = next;

                prev = curr;
                curr = curr->next;
            }
            /* If the page is already at the end of the queue, we're done
             * traversing.
             */
            else {
                break;
            }
        }

        /* If the page hasn't been accessed, continue traversing the queue */
        else {
            prev = curr;
            curr = curr->next;
        }
    }
}


/* Choose a page to evict from the collection of mapped pages.  Then, record
 * that it is evicted.  This is very simple since we are implementing a random
 * page-replacement policy.
 */
page_t choose_and_evict_victim_page(void) {
    /* Choose the page at the head of the queue to be the victim */
    page_t victim = loaded.head->page;

    /* Clean up the queue and free memory where appropriate */
    node_t *old = loaded.head;
    loaded.head = loaded.head->next;
    free(old);
    old = NULL;

#if VERBOSE
    fprintf(stderr, "Choosing victim page %u to evict.\n", victim);
#endif

    return victim;
}
