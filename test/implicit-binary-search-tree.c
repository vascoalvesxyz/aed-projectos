#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

#define RESIZE_FACTOR 1.6
#define SEED 95911405

#define AVERAGE 10
#define TREESIZE 1000

typedef uint32_t idx_t;
typedef int32_t key_t;
#define IDX_INVALID 4294967295

typedef struct RBNode {
    idx_t parent;   // 4 bytes
    idx_t left;     // 4 bytes
    idx_t right;    // 4 bytes
    key_t key;      // 4 bytes
    int8_t color;   // 1 bytes
} RBNode;

typedef struct RBTree {
    RBNode *nodes;
    idx_t tree_root;
    idx_t elements;
    idx_t capacity;
} RBTree;

extern RBTree  tree_rb_create(uint32_t initial_capacity);
extern void    tree_rb_destroy(RBTree *rb);
extern void    tree_rb_resize(RBTree *rb);
static int     _rb_is_red(RBTree *tree, idx_t i);
static idx_t   _rb_rotate_left(RBTree *tree, idx_t h);
static idx_t   _rb_rotate_right(RBTree *tree, idx_t h);
static void    _rb_flip_colors(RBTree *tree, idx_t h);
static idx_t   _rb_fix_up(RBTree *tree, idx_t h);
static idx_t   _rb_insert_recursive(RBTree *tree, idx_t h, key_t key);
extern void    tree_rb_insert(RBTree *tree, key_t key);
extern int     tree_rb_search(RBTree *rb, int key);

/* Criar arvore */
RBTree tree_rb_create(uint32_t initial_capacity) {
    RBTree tree;
    tree.nodes = malloc(initial_capacity * sizeof(RBNode));
    assert(tree.nodes != NULL);

    tree.elements = 0;
    tree.capacity = initial_capacity;

    // a raiz da arvore é invalida inicialmente
    // para que seja pintada correctamente de preto (caso especial)
    // e inserida imediatamente
    tree.tree_root = IDX_INVALID;

    RBNode *endptr = tree.nodes+tree.capacity;
    for (RBNode *ptr = tree.nodes; ptr != endptr; ptr++) {
        ptr->key = 0;
        ptr->color = -1;
        ptr->left = IDX_INVALID;
        ptr->right = IDX_INVALID;
    }
    return tree;
}

/* Destruir árvore */
void tree_rb_destroy(RBTree *rb) {
    free(rb->nodes);
}

/* Aumentar capacidade */
void tree_rb_resize(RBTree *tree) {
    assert(tree != NULL);
    uint32_t new_capacity = tree->capacity * RESIZE_FACTOR;
    
    RBNode *new_nodes = realloc(tree->nodes, new_capacity * sizeof(RBNode));
    if (new_nodes == NULL) {
        free(tree->nodes);
        perror("Failed to allocate more nodes.");
        exit(EXIT_FAILURE);
    }
    
    // inicializar novos nós
    RBNode *endptr = new_nodes + new_capacity;
    for (RBNode *ptr = new_nodes + tree->capacity; ptr != endptr; ptr++) {
        ptr->key = 0;
        ptr->color = -1;
        ptr->left = IDX_INVALID;
        ptr->right = IDX_INVALID;
    }
    
    tree->nodes = new_nodes;
    tree->capacity = new_capacity;
}
