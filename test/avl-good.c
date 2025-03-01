// C program to implement the avl tree
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
/*typedef struct {*/
/*    idx_t* stack;*/
/*    idx_t elements;*/
/*    idx_t capacity;*/
/*} IDX_Stack;*/

/*idx_t *free_idx_stack;*/

#define IDX_INVALID 4294967295
#define RESIZE_FACTOR 1.6108

typedef uint32_t idx_t;

typedef struct AVLNode {
    idx_t left;
    idx_t right;
    int key;
    int height;
} AVLNode;  // 16 bytes


typedef struct AVLTree {
    AVLNode *nodes;
    idx_t tree_root;
    idx_t elements;
    idx_t capacity;
} AVLTree;

/* Handling allocation. */
AVLTree tree_avl_create(idx_t inicial_capacity);
void    tree_avl_destroy(AVLTree* avl);
void    tree_avl_resize(AVLTree *avl);

/* Helper functions. */
static int   max(int a, int b);
static int   _tree_avl_get_height(AVLTree* avl, idx_t index);
static int   _tree_avl_get_balance(AVLTree* avl, idx_t index);
static idx_t _tree_avl_rotate_right(AVLTree *avl, idx_t y_index);
static idx_t _tree_avl_rotate_left(AVLTree *avl, idx_t x_index);
static idx_t _tree_avl_insert_recursive(AVLTree *avl, idx_t node_index, int key);

/* Basic operations */
void     tree_avl_insert(AVLTree *avl, int key);
AVLNode* tree_avl_search(AVLTree *avl, int key);
void     tree_avl_in_order(AVLTree *avl); // in-order print
/*void    tree_avl_remove(AVLTree *avl, int key);*/

AVLTree
tree_avl_create(idx_t inicial_capacity) {
    assert(inicial_capacity > 0);

    AVLTree avl = {NULL, 0, 0, inicial_capacity};
    avl.nodes = (AVLNode*) malloc( sizeof(AVLNode) * inicial_capacity);

    if (avl.nodes == NULL) {
        perror("Couldn't allocate AVL tree.");
        exit(EXIT_FAILURE);
    }

    for (idx_t i = 0; i < inicial_capacity; i++) {
        avl.nodes[i] = (AVLNode) {IDX_INVALID, IDX_INVALID, 0, 1};
    }

    return avl;
}

void
tree_avl_destroy(AVLTree* avl) {
    assert(avl);
    free(avl->nodes);
}

void
tree_avl_resize(AVLTree *avl) {
    idx_t new_capacity = avl->capacity*RESIZE_FACTOR;

    if (new_capacity <= avl->capacity) {
        perror("AVL tree exceeded maximum capacity.");
        exit(EXIT_FAILURE);
    }

    AVLNode* new_nodes = (AVLNode*) realloc(avl->nodes, sizeof(AVLNode)*new_capacity);

    if (new_nodes == NULL) {
        perror("Failed to allocate enough memory for tree resize.");
        exit(EXIT_FAILURE);
    }

    avl->nodes = new_nodes;        
    avl->capacity = new_capacity;
}


static int
max(int a, int b) {
    return (a > b) ? a : b; 
}

static int
_tree_avl_get_height(AVLTree* avl, idx_t index) {
    return (index == IDX_INVALID) ? 0 : avl->nodes[index].height;
}

static int
_tree_avl_get_balance(AVLTree* avl, idx_t index) {
    if (index == IDX_INVALID) return 0;
    return _tree_avl_get_height(avl, avl->nodes[index].left) - _tree_avl_get_height(avl, avl->nodes[index].right);
}

static idx_t
_tree_avl_rotate_right(AVLTree *avl, idx_t y_index) {
    idx_t x_index = avl->nodes[y_index].left;
    idx_t T2 = avl->nodes[x_index].right;

    // Perform rotation:
    avl->nodes[x_index].right = y_index;
    avl->nodes[y_index].left = T2;

    // Update heights:
    avl->nodes[y_index].height = 1 + max(_tree_avl_get_height(avl, avl->nodes[y_index].left), _tree_avl_get_height(avl, avl->nodes[y_index].right));
    avl->nodes[x_index].height = 1 + max(_tree_avl_get_height(avl, avl->nodes[x_index].left), _tree_avl_get_height(avl, avl->nodes[x_index].right));
    return x_index;
}

static idx_t
_tree_avl_rotate_left(AVLTree *avl, idx_t x_index) {
    idx_t y_index = avl->nodes[x_index].right;
    idx_t T2 = avl->nodes[y_index].left;

    // Perform rotation:
    avl->nodes[y_index].left = x_index;
    avl->nodes[x_index].right = T2;

    // Update heights:
    avl->nodes[x_index].height = 1 + max(_tree_avl_get_height(avl, avl->nodes[x_index].left), _tree_avl_get_height(avl, avl->nodes[x_index].right));
    avl->nodes[y_index].height = 1 + max(_tree_avl_get_height(avl, avl->nodes[y_index].left), _tree_avl_get_height(avl, avl->nodes[y_index].right));
    return y_index;
}

static idx_t
_tree_avl_insert_recursive(AVLTree *avl, idx_t node_index, int key) {
    
    if (node_index == IDX_INVALID) {
        idx_t new_index = avl->elements;
        AVLNode* new_node = &avl->nodes[new_index];
        new_node->key = key;
        new_node->left = IDX_INVALID;
        new_node->right = IDX_INVALID;
        new_node->height = 1;
        avl->elements++;
        return new_index;
    }
    
    /* Binary Search Tree */
    if (key < avl->nodes[node_index].key) {
        avl->nodes[node_index].left = _tree_avl_insert_recursive(avl, avl->nodes[node_index].left, key);
    } else if (key > avl->nodes[node_index].key) {
        avl->nodes[node_index].right = _tree_avl_insert_recursive(avl, avl->nodes[node_index].right, key);
    } else {
        return node_index;
    }
    
    avl->nodes[node_index].height = 1 + max(_tree_avl_get_height(avl, avl->nodes[node_index].left),
                                             _tree_avl_get_height(avl, avl->nodes[node_index].right));

    // Get balance factor to check if rebalancing is needed.
    int balance = _tree_avl_get_balance(avl, node_index);

    /* Left Left */
    if (balance > 1 && key < avl->nodes[avl->nodes[node_index].left].key)
        return _tree_avl_rotate_right(avl, node_index);

    /* Right Right */
    if (balance < -1 && key > avl->nodes[avl->nodes[node_index].right].key)
        return _tree_avl_rotate_left(avl, node_index);

    /* Left Right */
    if (balance > 1 && key > avl->nodes[avl->nodes[node_index].left].key) {
        avl->nodes[node_index].left = _tree_avl_rotate_left(avl, avl->nodes[node_index].left);
        return _tree_avl_rotate_right(avl, node_index);
    }

    /* Right Left */
    if (balance < -1 && key < avl->nodes[avl->nodes[node_index].right].key) {
        avl->nodes[node_index].right = _tree_avl_rotate_right(avl, avl->nodes[node_index].right);
        return _tree_avl_rotate_left(avl, node_index);
    }

    return node_index;
}

void
tree_avl_insert(AVLTree *avl, int key) {

    if (avl->elements == avl->capacity) {
        tree_avl_resize(avl);
    }


    /* For an empty tree, set the new node as root. */
    if (avl->elements == 0) {
        avl->tree_root = _tree_avl_insert_recursive(avl, IDX_INVALID, key);
    } else {
        avl->tree_root = _tree_avl_insert_recursive(avl, avl->tree_root, key);
    }
}

void
tree_avl_in_order(AVLTree *avl) {

    void _traverse(AVLNode *nodes, idx_t i) {
        if (i == IDX_INVALID) return;
        AVLNode no = nodes[i];
        _traverse(nodes, no.left);
        printf("%d ", no.key);
        _traverse(nodes, no.right);
    }

    _traverse(avl->nodes, avl->tree_root);
    puts("");
}

void
tree_avl_in_order_non(AVLTree *avl) {

    AVLNode current_node; 
    idx_t current_index = avl->tree_root;

    while (current_index != IDX_INVALID) {
        current_node = avl->nodes[avl->tree_root];

        /* Traverse left sub-tree until leaf */
        if (current_node.left != IDX_INVALID) {
            current_index = current_node.left;
            continue;
        }
        printf("%d ", current_node.key);
    }
}


AVLNode*
tree_avl_search(AVLTree *avl, int key) {
    idx_t current_index = avl->tree_root;
    while (current_index != IDX_INVALID) {
        printf("Current Index: %d\n", current_index);
        if (avl->nodes[current_index].key == key)
            return &avl->nodes[current_index];
            
        else if (key < avl->nodes[current_index].key)
            current_index = avl->nodes[current_index].left;

        else
            current_index = avl->nodes[current_index].right;
    }
    return NULL;  // Key not found.
}

int
main() {
    AVLTree avl = tree_avl_create(10);

    for (int i = 0; i < 1000000; i++) {
        tree_avl_insert(&avl, i);
    }

    printf("Inorder traversal of AVL tree: ");
    tree_avl_in_order(&avl);

    AVLNode* search = tree_avl_search(&avl, 1);
    if (search)
        printf("Found key %d = %d\n", 1, search->key);
    else
        puts("Not found");
    
    printf("avl.capacity = %d\n", avl.capacity);

    tree_avl_destroy(&avl);
    return 0;
}
