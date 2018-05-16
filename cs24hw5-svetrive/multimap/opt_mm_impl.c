#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "multimap.h"


/*============================================================================
 * TYPES
 *
 *   These types are defined in the implementation file so that they can
 *   be kept hidden to code outside this source file.  This is not for any
 *   security reason, but rather just so we can enforce that our testing
 *   programs are generic and don't have any access to implementation details.
 *============================================================================*/


/* Represents a key and its associated values in the multimap, as well as
 * pointers to the left and right child nodes in the multimap. */
typedef struct multimap_node {
    /* The key-value that this multimap node represents. */
    int key;

    /* An array of the values associated with this key in the multimap. */
    int *values;

    /* Number of values stored in the array */
    int num_vals;

    /* Size of the array */
    int size;

    /* The left child of the multimap node.  This will reference nodes that
     * hold keys that are strictly less than this node's key.
     */
    struct multimap_node *left_child;

    /* The right child of the multimap node.  This will reference nodes that
     * hold keys that are strictly greater than this node's key.
     */
    struct multimap_node *right_child;
} multimap_node;


/* The entry-point of the multimap data structure. */
struct multimap {
    multimap_node *root;
};

// pointer to beginning of array of memory slabs for nodes
void **mem;
// number of slabs in array of slabs
int num_slabs;

// number of slabs used to store nodes
int total_slabs;

// number of nodes in the most recent unfilled slabs
int num_nodes;

// number of nodes stored in each slab
int slab_size = 5000000;


/*============================================================================
 * HELPER FUNCTION DECLARATIONS
 *
 *   Declarations of helper functions that are local to this module.  Again,
 *   these are not visible outside of this module.
 *============================================================================*/

multimap_node * alloc_mm_node();

multimap_node * find_mm_node(multimap_node *root, int key,
                             int create_if_not_found);

void free_multimap_values(int *values);
void free_multimap_node(multimap_node *node);
void resize_arrays(multimap_node *node);
void new_slab();
void resize_slab_array();

/*============================================================================
 * FUNCTION IMPLEMENTATIONS
 *============================================================================*/

/* This function creates a new slab for us to store nodes and is used
 * if we filled up a previous slab or if we have not yet allocated memory
 * for the root node of the tree
 */
void new_slab() {
  if (num_slabs == total_slabs) {
    resize_slab_array();
  }
  mem[num_slabs] = malloc(slab_size * sizeof(multimap_node));
  num_slabs++;
  num_nodes = 0;
}

/* This is a helper function for the above new slab function. This resizes the
 * array containing the differest slabs of memory contains the nodes. If the
 * array needs to be made larger, we allocated memory to double the size of the
 * array and then move the data
 */
void resize_slab_array() {
  // check if the number of values has been initialized
  if (total_slabs) {
    // Allocate the slab
    void **new_mem = malloc(sizeof(void *) * total_slabs * 2);
    // Copy over information
    for (int i = 0; i < total_slabs; i++) {
      new_mem[i] = mem[i];
    }
    // Set new information
    free(mem);
    mem = new_mem;
    total_slabs = total_slabs * 2;
  }
  // Our first allocation!
  else {
    total_slabs = 1;
    mem = malloc(total_slabs * sizeof(void *));
  }
}

/* This helper function resizes the array used to store the values for a key
 * (node). If a key has more values that we have room for in the array, we
 * allocated memory to fit twice as many values and then copy over the
 * information, and assign this new array to the key to store the values for
 * node
 */
void resize_arrays(multimap_node *node) {
   // check if the number of values has been initialized
   if (node->num_vals) {
     // Create new array of double the size
     int * new_values = malloc(sizeof(int) * node->size * 2);
     // Copy over information
     for (int i = 0; i < node->num_vals; i++) {
       new_values[i] = node->values[i];
     }
     // Set new information
     free(node->values);
     node->values = new_values;
     node->size = 2 * node->size;
   }
   else {
     // Our first value for this key!
     node->size = 1;
     node->values = malloc(sizeof(int));
   }
}


/* Allocates a multimap node, and zeros out its contents so that we know what
 * the initial value of everything will be.
 */
multimap_node * alloc_mm_node() {
    // Check if we need a new slab to store nodes
    if (num_slabs == 0 || num_nodes*sizeof(multimap_node) >= slab_size) {
      new_slab();
    }
    // Now we have at least one slab, and space for our node!
    multimap_node *node = mem[num_slabs - 1] +
                          (num_nodes * sizeof(multimap_node));
    num_nodes++;
    bzero(node, sizeof(multimap_node));
    return node;
}


/* This helper function searches for the multimap node that contains the
 * specified key.  If such a node doesn't exist, the function can initialize
 * a new node and add this into the structure, or it will simply return NULL.
 * The one exception is the root - if the root is NULL then the function will
 * return a new root node.
 */
multimap_node * find_mm_node(multimap_node *root, int key,
                             int create_if_not_found) {
    multimap_node *node;

    /* If the entire multimap is empty, the root will be NULL. */
    if (root == NULL) {
        if (create_if_not_found) {
            root = alloc_mm_node();
            root->key = key;
        }
        return root;
    }

    /* Now we know the multimap has at least a root node, so start there. */
    node = root;
    while (1) {
        if (node->key == key)
            break;

        if (node->key > key) {   /* Follow left child */
            if (node->left_child == NULL && create_if_not_found) {
                /* No left child, but caller wants us to create a new node. */
                multimap_node *new = alloc_mm_node();
                new->key = key;

                node->left_child = new;
            }
            node = node->left_child;
        }
        else {                   /* Follow right child */
            if (node->right_child == NULL && create_if_not_found) {
                /* No right child, but caller wants us to create a new node. */
                multimap_node *new = alloc_mm_node();
                new->key = key;

                node->right_child = new;
            }
            node = node->right_child;
        }

        if (node == NULL)
            break;
    }

    return node;
}


/* This helper function frees all values in a multimap node's value-list. */
void free_multimap_values(int *values) {
    // Just free the pointer to the array
    if (values != NULL) {
        free(values);
    }
}


/* This helper function frees a multimap node, including its children and
 * value-list.
 */
void free_multimap_node(multimap_node *node) {
    if (node == NULL)
        return;

    /* Free the children first. */
    free_multimap_node(node->left_child);
    free_multimap_node(node->right_child);

    free_multimap_values(node->values);

#ifdef DEBUG_ZERO
    /* Clear out what we are about to free, to expose issues quickly. */
    bzero(node, sizeof(multimap_node));
#endif
}


/* Initialize a multimap data structure. */
multimap * init_multimap() {
    multimap *mm = malloc(sizeof(multimap));
    mm->root = NULL;
    return mm;
}


/* Release all dynamically allocated memory associated with the multimap
 * data structure.
 */
void clear_multimap(multimap *mm) {
    assert(mm != NULL);
    free_multimap_node(mm->root);
    mm->root = NULL;

    // Need to free the array of slabs for the nodes
    for (int i = 0; i < num_slabs; i++) {
      free(mem[i]);
    }

    // Reset all global variables
    mem = NULL;
    num_slabs = 0;
    total_slabs = 0;
    num_nodes = 0;
}


/* Adds the specified (key, value) pair to the multimap. */
void mm_add_value(multimap *mm, int key, int value) {
    multimap_node *node;

    assert(mm != NULL);

    /* Look up the node with the specified key.  Create if not found. */
    node = find_mm_node(mm->root, key, /* create */ 1);
    if (mm->root == NULL)
        mm->root = node;

    assert(node != NULL);
    assert(node->key == key);

    /* Add the new value to the multimap node. */
    if (node->num_vals >= node->size) {
      resize_arrays(node);
    }
    // Update the node values since we have added a value
    node->values[node->num_vals] = value;
    node->num_vals++;
}


/* Returns nonzero if the multimap contains the specified key-value, zero
 * otherwise.
 */
int mm_contains_key(multimap *mm, int key) {
    return find_mm_node(mm->root, key, /* create */ 0) != NULL;
}


/* Returns nonzero if the multimap contains the specified (key, value) pair,
 * zero otherwise.
 */
int mm_contains_pair(multimap *mm, int key, int value) {
    multimap_node *node;

    node = find_mm_node(mm->root, key, /* create */ 0);
    if (node == NULL)
        return 0;

    // Loop through all values for this node to find the value.
    for (int i = 0; i < node->num_vals; i++) {
      if (node->values[i] == value) {
        return 1;
      }
    }
    return 0;
}


/* This helper function is used by mm_traverse() to traverse every pair within
 * the multimap.
 */
void mm_traverse_helper(multimap_node *node, void (*f)(int key, int value)) {

    if (node == NULL)
        return;

    mm_traverse_helper(node->left_child, f);

    // Loop through all values for this node to run the function.
    for (int i = 0; i < node->num_vals; i++) {
      f(node->key, node->values[i]);
    }

    mm_traverse_helper(node->right_child, f);
}


/* Performs an in-order traversal of the multimap, passing each (key, value)
 * pair to the specified function.
 */
void mm_traverse(multimap *mm, void (*f)(int key, int value)) {
    mm_traverse_helper(mm->root, f);
}
