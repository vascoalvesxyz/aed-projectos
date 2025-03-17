#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define RESIZE_FACTOR 1.618
#define IDX_INVALID 4294967295
#define SEED 95911405

#define AVERAGE 10
#define TREESIZE 1000

typedef uint32_t idx_t;
typedef int32_t key_t;

typedef struct TreapNode {
    key_t key;
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

extern Treap tree_treap_create(int inicial_capacity);
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
        b ^= a ;
        a ^= b;
    }
    return a + rand() % (b - a + 1);
}

Treap
tree_treap_create(int inicial_capacity) {
    Treap new_treap;
    new_treap.nodes = (TreapNode*) malloc( sizeof(TreapNode) * inicial_capacity );
    if (new_treap.nodes == NULL) {
        perror("Failed to allocated Treap.");
        exit(EXIT_FAILURE);
    }

    new_treap.tree_root = IDX_INVALID; // garante
    new_treap.elements = 0;
    new_treap.capacity = inicial_capacity;

    TreapNode* end = new_treap.nodes + inicial_capacity;
    for (TreapNode *ptr = new_treap.nodes; ptr != end; ptr++) {
        *ptr = (TreapNode) {0, 0, IDX_INVALID, IDX_INVALID};
    }

    return new_treap;
}

void
tree_treap_resize(Treap *treap) {
    /* Do nothing if at max capacity */
    if (treap->capacity == IDX_INVALID-1) return;

    idx_t new_capacity = treap->capacity * RESIZE_FACTOR;

    /* no caso de haver overflow */
    if (new_capacity < treap->capacity) {
        new_capacity = IDX_INVALID-1; // maximum capacity
    }

    /* realloc */
    TreapNode *new_nodes = (TreapNode*) realloc(treap->nodes, sizeof(TreapNode)*new_capacity);
    if (new_nodes == NULL) {
        perror("Failed to realloc new nodes.");
        exit(EXIT_FAILURE);
    }

    treap->nodes = new_nodes;

    /* inicializar */
    TreapNode *end = treap->nodes + new_capacity;
    for (TreapNode *ptr = treap->nodes+treap->capacity+1; ptr != end; ptr++) {
        *ptr = (TreapNode) {0, 0, IDX_INVALID, IDX_INVALID};
    }

    treap->capacity = new_capacity;
}

void
tree_treap_destroy(Treap *treap) {
    free(treap->nodes);
    treap->capacity = 0;
    treap->elements = 0;
}

/* Right rotation implementation */
static idx_t _treap_rotate_right(Treap *treap, idx_t y_idx) {
    TreapNode *nodes = treap->nodes;
    TreapNode *y = &nodes[y_idx];
    idx_t x_idx = y->left;
    TreapNode *x = &nodes[x_idx];
    
    /* Perform rotation */
    y->left = x->right;
    x->right = y_idx;
    
    /* Return new root */
    return x_idx;
}

/* Left rotation implementation */
static idx_t _treap_rotate_left(Treap *treap, idx_t x_idx) {
    TreapNode *nodes = treap->nodes;
    TreapNode *x = &nodes[x_idx];
    idx_t y_idx = x->right;
    TreapNode *y = &nodes[y_idx];
    
    /* Perform rotation */
    x->right = y->left;
    y->left = x_idx;
    
    /* Return new root */
    return y_idx;
}

static idx_t
_treap_insert_recursive(Treap *treap, idx_t idx, key_t key) {
    
    TreapNode *nodes = treap->nodes;

    /* a ultima chamada atingiu um nó que ainda não existe 
     * vamos criar este nó e devolver o indice novo */
    if (idx == IDX_INVALID) {
        idx_t new_index = treap->elements;
        treap->elements++;

        nodes[new_index] = (TreapNode) {
            .key = key,
            .priority = randint(1, IDX_INVALID-1),
            .left = IDX_INVALID,
            .right = IDX_INVALID
        };
        return new_index;
    }

    TreapNode no = nodes[idx];

    if ( key < no.key ) {
        no.left = _treap_insert_recursive(treap, no.left, key);
        /* treap property check and rotation */
        if (nodes[no.left].priority > no.priority) {
            idx = _treap_rotate_right(treap, idx);
        }
    }
    else if (key > no.key) {
        no.right = _treap_insert_recursive(treap, no.right, key);
        if (nodes[no.right].priority > no.priority) {
            idx = _treap_rotate_left(treap, idx);
        }
    } 
    /* se a key for igual não fazemos nada */

    return idx;
}

void
tree_treap_insert(Treap *treap, key_t key) {
    /* resize se for necessário */
    if (treap->capacity == treap->elements)    
        tree_treap_resize(treap);

    treap->tree_root = _treap_insert_recursive(treap, treap->tree_root, key);


}

int main() {
    Treap treap = tree_treap_create(TREESIZE);
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
    tree_treap_destroy(&treap);
    return 0;
}
