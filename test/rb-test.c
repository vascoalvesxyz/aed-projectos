#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

#define RESIZE_FACTOR 2
#define IDX_INVALID 4294967295
#define SEED 95911405

#define AVERAGE 10
#define TREESIZE 1000

#define BLACK 0
#define RED 1
#define LEFT 0
#define RIGHT 1

typedef uint32_t idx_t;
typedef int32_t key_t;

typedef struct RBNode {
    idx_t parent;   // 4 bytes
    idx_t left;     // 4 bytes
    idx_t right;    // 4 bytes
    key_t key;      // 4 bytes
    int8_t color;   // 1 bytes
} RBNode;

typedef struct RBTree {
    RBNode *nodes;
    RBNode *tree_root;
    idx_t elements;
    idx_t capacity;
} RBTree;

RBTree  tree_rb_create(uint32_t initial_capacity);
void    tree_rb_destroy(RBTree *rb);
void    tree_rb_resize(RBTree *rb);
int     tree_rb_insert(RBTree *rb, int key);
int     tree_rb_search(RBTree *rb, int key);

void rb_rotate(RBTree *rb, RBNode *subtree, int dir) {
    assert(dir == LEFT || dir == RIGHT); // duh
    
    idx_t subtree_index = subtree - rb->nodes;
    RBNode* pivot;

    idx_t pivot_index = (dir==LEFT) ? subtree->right : subtree->left;
    pivot_index = subtree->right;

    if (pivot_index == IDX_INVALID) {
        perror("RBTree: Invalid Rotationn");
        return;
    }

    /* Left Rotation */
    if (dir == LEFT) {

        pivot = &rb->nodes[pivot_index];
        subtree->right = pivot->left;

        if (pivot->left != IDX_INVALID) {
            rb->nodes[pivot->left].parent = subtree_index;
        }

        pivot->left = subtree_index;
    } 
    /* Right Rotation*/
    else { /* é sempre 1 ou 0 por causa do assert */

        pivot_index = subtree->left;
        pivot = &rb->nodes[pivot_index];

        subtree->left = pivot->right;
        if (pivot->right != IDX_INVALID) {
            rb->nodes[pivot->right].parent = subtree_index;
        }

        pivot->right = subtree_index;
    } 

    /* Update parent pointers. */
    idx_t parent_index = subtree->parent;
    pivot->parent = parent_index;
    subtree->parent = pivot_index;

    /* New root */
    if (parent_index == IDX_INVALID) {
        rb->tree_root = pivot;
    } else {
        RBNode* parent = &rb->nodes[parent_index];
        if (parent->left == subtree_index) {
            parent->left = pivot_index;
        } else {
            parent->right = pivot_index;
        }
    }
}

RBTree
tree_rb_create(uint32_t initial_capacity) {
    RBTree tree;
    tree.nodes = malloc(initial_capacity * sizeof(RBNode));
    assert(tree.nodes != NULL);

    tree.elements = 0;
    tree.capacity = initial_capacity;
    tree.tree_root = NULL;

    RBNode *endptr = tree.nodes+tree.capacity;
    for (RBNode *ptr = tree.nodes; ptr != endptr; ptr++) {
        ptr->key = 0;
        ptr->color = -1;
        ptr->left = IDX_INVALID;
        ptr->right = IDX_INVALID;
    }
    return tree;
}

void
tree_rb_destroy(RBTree *rb) {
    free(rb->nodes);
}

void
tree_rb_resize(RBTree *tree) {
    assert(tree != NULL);
    uint32_t new_capacity = tree->capacity * RESIZE_FACTOR;
    
    RBNode *new_nodes = realloc(tree->nodes, new_capacity * sizeof(RBNode));
    if (new_nodes == NULL) {
        free(tree->nodes);
        perror("Failed to allocate more nodes.");
        exit(EXIT_FAILURE);
    }
    
    // Initialize the new portion of the array using new_nodes.
    RBNode *endptr = new_nodes + new_capacity;
    for (RBNode *ptr = new_nodes + tree->capacity; ptr != endptr; ptr++) {
        ptr->key = 0;
        ptr->color = -1;
        ptr->left = IDX_INVALID;
        ptr->right = IDX_INVALID;
    }
    
    if ( tree->tree_root == tree->nodes ) 
        tree->tree_root == new_nodes;

    tree->nodes = new_nodes;
    tree->capacity = new_capacity;
}

int
tree_rb_insert(RBTree *tree, int key) {
    assert(tree != NULL);

    /* alocar memoria se for preciso */
    if (tree->elements == tree->capacity)
        tree_rb_resize(tree);
    printf("tree->capacity = %d\n", tree->capacity);
    printf("tree->elemens = %d\n", tree->elements);

    RBNode *nodes = tree->nodes;
    RBNode this_node;
    idx_t current = (tree->tree_root != NULL) ? (tree->tree_root - nodes) : IDX_INVALID;
    idx_t parent = IDX_INVALID;

    /* binary search tree insert */
    while (current != IDX_INVALID) {
        parent = current;
        printf("current = %d\n", current);
        if (key < nodes[current].key)
            current = nodes[current].left;
        else if (key > nodes[current].key)
            current = nodes[current].right;
        else
            return 1;  // duplicate key, do not insert
    }

    /* inserir novo nó */
    RBNode *new_node = &nodes[tree->elements];
    printf("new_node idx = %d\n", new_node-nodes);
    tree->elements++;
    new_node->key = key;
    new_node->color = RED;
    new_node->left = IDX_INVALID;
    new_node->right = IDX_INVALID;
    new_node->parent = parent;

    /* Pais e filhos */
    if (parent == IDX_INVALID) {
        /*tree->tree_root = new_node;*/
        new_node->color = BLACK; // caso especial
    } else if (new_node->key < nodes[parent].key) {
        (nodes+parent)->left = nodes-new_node; // idx
    } else {
        (nodes+parent)->right = nodes-new_node; // idx
    }

    /* Os primeiros dois nós não precisam de fix x*/
    if (parent!=IDX_INVALID && nodes[new_node->parent].parent == IDX_INVALID) {
        return 0;
    }

    /*fixInsert(new_node);*/
    return 0;
}

int tree_rb_search(RBTree *tree, int key) {
    assert(tree != NULL);
    return -1;
}

int main(void) {

    RBTree tree = tree_rb_create(TREESIZE);

    int keys[] = {7, 9, 4, 11, 3, 5};
    size_t num_keys = sizeof(keys) / sizeof(keys[0]);

    for (size_t i = 0; i < num_keys; i++) {
        if (0==tree_rb_insert(&tree, keys[i])) {
            printf("Inserted key %d\n", keys[i]);
        } else {
            printf("Key %d is a duplicate and was not inserted.\n", keys[i]);
        }
    }

    printf("RB-tree built with %u nodes.\n", tree.elements);

    /* Search for some keys */
    /*int search_keys[] = {11, 5, 100};*/
    /*size_t num_search = sizeof(search_keys) / sizeof(search_keys[0]);*/
    /*for (size_t i = 0; i < num_search; i++) {*/
    /*    int idx = tree_rb_search(&tree, search_keys[i]);*/
    /*    if (idx != -1)*/
    /*        printf("Found key %d at node index %d\n", search_keys[i], idx);*/
    /*    else*/
    /*        printf("Key %d not found in the tree.\n", search_keys[i]);*/
    /*}*/

    tree_rb_destroy(&tree);
    return 0;
}
