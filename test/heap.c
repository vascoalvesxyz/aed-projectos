#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define RESIZE_FACTOR 1.618
#define IDX_INVALID 4294967295U
#define SEED 95911405

#define TREESIZE 1000

typedef uint32_t idx_t;
typedef int32_t key_t;

typedef struct TreapNode {
    key_t value;
    idx_t priority;
    idx_t left;
    idx_t right;
} TreapNode;

typedef struct Treap {
    TreapNode* nodes;
    idx_t tree_root;
    idx_t elements;
    idx_t capacity;
} Treap;

static inline int randint(int a, int b);

/* Function prototypes */
extern Treap tree_treap_create(idx_t initial_capacity);
extern void  tree_treap_resize(Treap *treap);
extern void  tree_treap_destroy(Treap *treap);
static idx_t _treap_rotate_right(Treap *treap, idx_t x_idx);
static idx_t _treap_rotate_left(Treap *treap, idx_t x_idx);
static idx_t _treap_insert_recursive(Treap *treap, idx_t idx, key_t key);
extern void  tree_treap_insert(Treap *treap, key_t key);

static inline int 
randint(int a, int b) {
    if (a > b) {
        a ^= b;
        b ^= a;
        a ^= b;
    }
    return a + rand() % (b - a + 1);
}

Treap tree_treap_create(idx_t initial_capacity) {
    Treap new_treap;
    new_treap.nodes = (TreapNode*) malloc(sizeof(TreapNode) * initial_capacity);
    if (new_treap.nodes == NULL) {
        perror("Failed to allocate Treap.");
        exit(EXIT_FAILURE);
    }

    new_treap.tree_root = IDX_INVALID;
    new_treap.elements = 0;
    new_treap.capacity = initial_capacity;

    // Initialize all nodes to default values.
    for (idx_t i = 0; i < initial_capacity; i++) {
        new_treap.nodes[i] = (TreapNode){0, 0, IDX_INVALID, IDX_INVALID};
    }

    return new_treap;
}

void tree_treap_resize(Treap *treap) {
    if (treap->capacity == IDX_INVALID - 1) return; // Already at max capacity

    idx_t old_capacity = treap->capacity;
    idx_t new_capacity = (idx_t)(treap->capacity * RESIZE_FACTOR);

    if (new_capacity < treap->capacity) {
        new_capacity = IDX_INVALID - 1; // Handle overflow: maximum capacity
    }

    TreapNode *new_nodes = (TreapNode*) realloc(treap->nodes, sizeof(TreapNode) * new_capacity);
    if (new_nodes == NULL) {
        perror("Failed to realloc new nodes.");
        exit(EXIT_FAILURE);
    }
    treap->nodes = new_nodes;

    // Initialize new memory area.
    for (idx_t i = old_capacity; i < new_capacity; i++) {
        treap->nodes[i] = (TreapNode){0, 0, IDX_INVALID, IDX_INVALID};
    }

    treap->capacity = new_capacity;
}

/* Free the allocated memory for the treap */
void tree_treap_destroy(Treap *treap) {
    free(treap->nodes);
    treap->capacity = 0;
    treap->elements = 0;
}

/* Right rotation
 *        y               x
 *       / \     ==>     / \
 *      x   C           A   y
 *     / \                 / \
 *    A   B               B   C
 */
static idx_t _treap_rotate_right(Treap *treap, idx_t y_idx) {
    TreapNode *nodes = treap->nodes;
    idx_t x_idx = nodes[y_idx].left;
    // Rotate: move x's right subtree to y's left subtree.
    nodes[y_idx].left = nodes[x_idx].right;
    nodes[x_idx].right = y_idx;
    return x_idx;
}

/* Left rotation
 *     x                    y
 *    / \       ==>        / \
 *   A   y                x   C
 *      / \              / \
 *     B   C            A   B
 */
static idx_t _treap_rotate_left(Treap *treap, idx_t x_idx) {
    TreapNode *nodes = treap->nodes;
    idx_t y_idx = nodes[x_idx].right;
    // Rotate: move y's left subtree to x's right subtree.
    nodes[x_idx].right = nodes[y_idx].left;
    nodes[y_idx].left = x_idx;
    return y_idx;
}

/* Recursive insertion
 * This function inserts a key into the treap subtree rooted at idx and returns the new root index.
 */
static idx_t _treap_insert_recursive(Treap *treap, idx_t idx, key_t key) {
    TreapNode *nodes = treap->nodes;

    // If we reached an empty spot, create a new node.
    if (idx == IDX_INVALID) {
        idx_t new_index = treap->elements;
        treap->elements++;
        nodes[new_index] = (TreapNode){
            .value = key,
            .priority = (idx_t)randint(1, IDX_INVALID - 1),
            .left = IDX_INVALID,
            .right = IDX_INVALID
        };
        return new_index;
    }

    // BST insertion: compare key with the current node's value.
    if (key < nodes[idx].value) {
        nodes[idx].left = _treap_insert_recursive(treap, nodes[idx].left, key);
        // Heap property: if left child's priority is greater, rotate right.
        if (nodes[nodes[idx].left].priority > nodes[idx].priority) {
            idx = _treap_rotate_right(treap, idx);
        }
    } else if (key > nodes[idx].value) {
        nodes[idx].right = _treap_insert_recursive(treap, nodes[idx].right, key);
        // Heap property: if right child's priority is greater, rotate left.
        if (nodes[nodes[idx].right].priority > nodes[idx].priority) {
            idx = _treap_rotate_left(treap, idx);
        }
    }
    // If key is equal, do nothing (no duplicates) or update as needed.

    return idx;
}

/* Public insertion function */
void tree_treap_insert(Treap *treap, key_t key) {
    // Resize if needed.
    if (treap->capacity == treap->elements)    
        tree_treap_resize(treap);
    treap->tree_root = _treap_insert_recursive(treap, treap->tree_root, key);
}

int main() {
    srand(SEED);  // Initialize random number generator for reproducibility

    Treap treap = tree_treap_create(TREESIZE);
    
    // Insert some keys.
    tree_treap_insert(&treap, 1);
    tree_treap_insert(&treap, 2);
    tree_treap_insert(&treap, 3);
    tree_treap_insert(&treap, 4);
    tree_treap_insert(&treap, 5);
    tree_treap_insert(&treap, 6);
    tree_treap_insert(&treap, 7);
    tree_treap_insert(&treap, 8);
    tree_treap_insert(&treap, 9);
    tree_treap_insert(&treap, 10);
    tree_treap_insert(&treap, 11);
    tree_treap_insert(&treap, 12);
    tree_treap_insert(&treap, 13);
    
    // Optional: Print or validate the treap structure here.

    tree_treap_destroy(&treap);
    puts("Done!");
    return 0;
}
