/*============================================================================
 * Implementation of the CLRU page replacement policy.
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
 * virtual memory as nodes.
 */

typedef struct node_t {
    /* Represents one page */
    page_t page;
    /*Points to another page*/
    struct node_t *next;
} node_t;

/*============================================================================
 * "Queue" Data Structure
 *
 * This data structure records pages that are currently loaded in the
 * virtual memory as a queue of nodes so we can easily remove the lest recently accessed page
 */

typedef struct queue_t {
    node_t * head;
    node_t * tail;
} queue_t;


/*============================================================================
 * Policy Implementation
 */


/* The list of pages that are currently resident. */
static queue_t loaded;


/* Initialize the policy.  Return nonzero for success, 0 for failure. */
int policy_init(int max_resident) {
    fprintf(stderr, "Using CLOCK/LRU eviction policy.\n\n");

    loaded.head = NULL;
    loaded.tail = NULL;

    /* Return nonzero if initialization succeeded. */
    return 1;
}


/* Clean up the data used by the page replacement policy. */
void policy_cleanup(void) {
}


/* This function is called when the virtual memory system maps a page into the
 * virtual address space.  Record that the page is now resident.
 */
void policy_page_mapped(page_t page) {
    // Allocate memory for a node for this page
    node_t * n = (node_t *) malloc(sizeof(node_t));
    n->page = page;
    n->next = NULL;

    // If our queue is currently empty
    if (loaded.head == NULL && loaded.tail == NULL) {
        loaded.head = n;
        loaded.tail = n;
    }
    // Add to end of queue
    else {
        loaded.tail->next = n;
        loaded.tail = n;
    }
    return;
}


/* This function is called when the virtual memory system has a timer tick. */
void policy_timer_tick(void) {

    // Pointers for our while loop and to help us move around nodes in the queue
    node_t * curr = loaded.head;
    node_t * prev = NULL;

    node_t * old_tail = loaded.tail;
    node_t * next;

    // While we haven't traversed through all elements in the queue
    while (prev != old_tail) {
        // If we have accessed the page since the last tick
        if (is_page_accessed(curr->page)) {
            // Clear the flag and set the permissions
            clear_page_accessed(curr->page);
            set_page_permission(curr->page, PAGEPERM_NONE);

            // We want to move this node to the end of the queue

            // This is not the last element in the queue
            if (curr->next) {
                // It's also not the first element
                if (prev) {
                    // We skip over curr in the queue
                    prev->next = curr->next;
                }
                else {
                    // The top of the queue moves to the next node
                    loaded.head = curr->next;
                }
                // We get the next node
                next = curr->next;
                // We set this next to null, since this will be the last node in the queue
                curr->next = NULL;
                // The last element is curr
                loaded.tail->next = curr;
                // Move the pointer
                loaded.tail = curr;
                // Move to the next element in the queue for our while loop
                curr = next;

                prev = curr;
                curr = curr->next;

            }
            else {
                break;
            }
        }
        else {
            // Keep traversing through queue
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
    page_t victim = loaded.head->page;

    /* Remove first node from queue (has not been accessed the longest). */
    node_t * first = loaded.head;
    loaded.head = loaded.head->next;
    free(first);
    first = NULL;


#if VERBOSE
    fprintf(stderr, "Choosing victim page %u to evict.\n", victim);
#endif

    return victim;
}
