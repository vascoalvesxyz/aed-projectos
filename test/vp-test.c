#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

#define INVALID_IDX UINT32_MAX

typedef struct VPNode {
    double threshold;    // distance threshold to partition left/right
    uint32_t left;       // index of left child (or INVALID_IDX)
    uint32_t right;      // index of right child (or INVALID_IDX)
    int key;             // stored key
} vp_node_t;

typedef struct VPTree {
    vp_node_t *nodes;    // dynamic array of nodes
    uint32_t elements;   // number of nodes currently in the tree
    uint32_t capacity;   // allocated capacity of the nodes array
} vp_tree_t;

static inline int distance(int a, int b);
vp_tree_t   vp_tree_create(uint32_t initial_capacity);
void        vp_tree_resize(vp_tree_t *tree, uint32_t new_capacity);
int         vp_tree_insert(vp_tree_t *tree, int key;
int         vp_tree_search(vp_tree_t *tree, int key);

/* Compute the absolute difference between two keys */
static inline int distance(int a, int b) {
    return (a > b) ? a - b : b - a;
}

/* Create a new VP‐tree with the given initial capacity.
   All node fields are initialized.
*/
vp_tree_t vp_tree_create(uint32_t initial_capacity) {
    vp_tree_t tree;
    tree.nodes = malloc(initial_capacity * sizeof(vp_node_t));
    assert(tree.nodes != NULL);
    tree.elements = 0;
    tree.capacity = initial_capacity;
    for (uint32_t i = 0; i < tree.capacity; i++) {
        tree.nodes[i].key = 0;
        tree.nodes[i].threshold = 0;
        tree.nodes[i].left = INVALID_IDX;
        tree.nodes[i].right = INVALID_IDX;
    }
    return tree;
}

/* Resize the tree's node array if needed. */
void vp_tree_resize(vp_tree_t *tree, uint32_t new_capacity) {
    assert(tree != NULL);
    vp_node_t *new_nodes = realloc(tree->nodes, new_capacity * sizeof(vp_node_t));
    if (new_nodes == NULL) {
        free(tree->nodes);
        perror("Failed to allocate more nodes.");
        exit(EXIT_FAILURE);
    }
    /* Initialize the new portion of the array */
    for (uint32_t i = tree->capacity; i < new_capacity; i++) {
        new_nodes[i].key = 0;
        new_nodes[i].threshold = 0;
        new_nodes[i].left = INVALID_IDX;
        new_nodes[i].right = INVALID_IDX;
    }
    tree->nodes = new_nodes;
    tree->capacity = new_capacity;
}

/* Insert a key into the VP‐tree.
   Returns 1 if the key was inserted or 0 if it was a duplicate.
   The insertion is done iteratively (non‐recursive).
*/
int vp_tree_insert(vp_tree_t *tree, int key) {
    assert(tree != NULL);
    
    /* If the tree is empty, insert the key as the root node. */
    if (tree->elements == 0) {
        if (tree->elements >= tree->capacity)
            vp_tree_resize(tree, tree->capacity * 2);
        tree->nodes[0].key = key;
        tree->nodes[0].threshold = 0; // no child yet
        tree->nodes[0].left = INVALID_IDX;
        tree->nodes[0].right = INVALID_IDX;
        tree->elements = 1;
        return 1;
    }
    
    uint32_t current = 0;
    while (1) {
        int current_key = tree->nodes[current].key;
        int d = distance(key, current_key);
        
        /* If the distance is zero, it's a duplicate key */
        if (d == 0)
            return 0;
        
        /* If current node is a leaf (no children), then insert as left child */
        if (tree->nodes[current].left == INVALID_IDX &&
            tree->nodes[current].right == INVALID_IDX) {
            if (tree->elements >= tree->capacity)
                vp_tree_resize(tree, tree->capacity * 2);
            uint32_t new_index = tree->elements;
            tree->nodes[new_index].key = key;
            tree->nodes[new_index].threshold = 0;
            tree->nodes[new_index].left = INVALID_IDX;
            tree->nodes[new_index].right = INVALID_IDX;
            tree->nodes[current].left = new_index;
            tree->nodes[current].threshold = d;
            tree->elements++;
            return 1;
        }
        
        /* If threshold is not yet set, set it (this case can happen if the node has one child) */
        if (tree->nodes[current].threshold == 0) {
            tree->nodes[current].threshold = d;
        }
        
        /* Decide which branch to follow */
        if (d <= tree->nodes[current].threshold) {
            if (tree->nodes[current].left == INVALID_IDX) {
                if (tree->elements >= tree->capacity)
                    vp_tree_resize(tree, tree->capacity * 2);
                uint32_t new_index = tree->elements;
                tree->nodes[new_index].key = key;
                tree->nodes[new_index].threshold = 0;
                tree->nodes[new_index].left = INVALID_IDX;
                tree->nodes[new_index].right = INVALID_IDX;
                tree->nodes[current].left = new_index;
                /* Update threshold if needed */
                if (d > tree->nodes[current].threshold)
                    tree->nodes[current].threshold = d;
                tree->elements++;
                return 1;
            } else {
                current = tree->nodes[current].left;
            }
        } else {
            if (tree->nodes[current].right == INVALID_IDX) {
                if (tree->elements >= tree->capacity)
                    vp_tree_resize(tree, tree->capacity * 2);
                uint32_t new_index = tree->elements;
                tree->nodes[new_index].key = key;
                tree->nodes[new_index].threshold = 0;
                tree->nodes[new_index].left = INVALID_IDX;
                tree->nodes[new_index].right = INVALID_IDX;
                tree->nodes[current].right = new_index;
                tree->elements++;
                return 1;
            } else {
                current = tree->nodes[current].right;
            }
        }
    }
}

/* Search for a key in the VP‐tree.
   Returns the index of the node containing the key if found, or -1 if not found.
   This function is also implemented iteratively.
*/
int vp_tree_search(vp_tree_t *tree, int key) {
    assert(tree != NULL);
    if (tree->elements == 0)
        return -1;
    
    uint32_t current = 0;
    while (current != INVALID_IDX) {
        int current_key = tree->nodes[current].key;
        if (current_key == key)
            return (int)current;
        
        int d = distance(key, current_key);
        /* If threshold is zero then this is a leaf and the key is not here */
        if (tree->nodes[current].threshold == 0)
            return -1;
        
        if (d <= tree->nodes[current].threshold)
            current = tree->nodes[current].left;
        else
            current = tree->nodes[current].right;
    }
    return -1;
}

/* Example main function demonstrating insertion and search */
int main(void) {
    /* Create a VP-tree with an initial capacity */
    vp_tree_t tree = vp_tree_create(3);
    
    /* Array of keys to insert (duplicates will not be inserted) */
    int keys[] = {7, 9, 4, 11, 3, 5};
    size_t num_keys = sizeof(keys) / sizeof(keys[0]);

    /* Insert each key */
    for (size_t i = 0; i < num_keys; i++) {
        if (vp_tree_insert(&tree, keys[i])) {
            printf("Inserted key %d\n", keys[i]);
        } else {
            printf("Key %d is a duplicate and was not inserted.\n", keys[i]);
        }
    }
    printf("VP-tree built with %u nodes.\n", tree.elements);

    /* Search for some keys */
    int search_keys[] = {11, 5, 100};
    size_t num_search = sizeof(search_keys) / sizeof(search_keys[0]);
    for (size_t i = 0; i < num_search; i++) {
        int idx = vp_tree_search(&tree, search_keys[i]);
        if (idx != -1)
            printf("Found key %d at node index %d\n", search_keys[i], idx);
        else
            printf("Key %d not found in the tree.\n", search_keys[i]);
    }
    
    /* Free allocated memory */
    free(tree.nodes);
    return 0;
}
