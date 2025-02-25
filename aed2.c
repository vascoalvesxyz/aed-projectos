#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#define TREE_BINARY_DARRAY_GROWTH 16
#define ROOT_IDX 0
#define LEFT 0
#define RIGHT 1

typedef struct BinTreeNode {
    int32_t data;
    uint32_t idx_left;
    uint32_t idx_right;
} BinTreeNode ;

typedef uint32_t tree_idx;
typedef struct BinTreeNode BinaryTreeRoot;

typedef struct BinaryTree {
    uint32_t capacity;
    uint32_t elements;
    BinaryTreeRoot *root;
} BinTree ;

extern int randint(int a, int b);

extern BinTree  tree_binary_create(uint16_t initial_capacity, int32_t data); // Creates binary tree
extern void     tree_binary_destroy(BinTree btree); // Frees binary tree
extern int      tree_binary_resize(BinTree *btree); 
extern void     tree_binary_init(tree_idx node, int32_t data);
extern void     tree_binary_insert(BinTree btree, tree_idx where, tree_idx node, int dir);
extern tree_idx tree_binary_get_new_elem(BinTree *btree, int32_t data);
extern void     tree_binary_insert(BinTree btree, tree_idx where, tree_idx node, int dir);
extern void     tree_binary_print(BinTree *btree);
extern void     tree_binary_fill_randomly(BinTree *btree);

int 
randint(int a, int b) {
    if (a > b) {
        a ^= b;
        b ^= a ;
        a ^= b;
    }
    return a + rand() % (b - a + 1);
}

BinTree
tree_binary_create(uint16_t initial_capacity, int32_t data) {
    BinTree btree = {initial_capacity, 0, NULL};
    btree.root = malloc(sizeof(BinTreeNode)*initial_capacity);
    btree.root->data = data;
    return btree;
}

void
tree_binary_destroy(BinTree btree) {
    free(btree.root);
}

int
tree_binary_resize(BinTree *btree) {
    uint16_t new_capacity = btree->capacity + TREE_BINARY_DARRAY_GROWTH;
    BinaryTreeRoot* new_root = NULL; 

    while (new_root == NULL) {
        new_root = (BinaryTreeRoot*) realloc(btree->root, sizeof(BinTreeNode)*new_capacity);
    }

    if (new_root) {
        btree->capacity = new_capacity;
        btree->root = new_root;
        return 0;
    }

    return 1;
}

void
tree_binary_insert(BinTree btree, tree_idx where, tree_idx what, int dir) {
    if (btree.elements >= btree.capacity) return;

    BinTreeNode* node = btree.root + where;
    if (node) {
        if (dir == 0) {
            node->idx_left = what; 
        } else if (dir == 1) {
            node->idx_right = what; 
        }
    }
    return;
}

tree_idx
tree_binary_get_new_elem(BinTree *btree, int32_t data) {

    int resize_success = 0;
    if (btree->elements >= btree->capacity) {
        resize_success = tree_binary_resize(btree);
    }

    if (resize_success == 0) {
        btree->elements += 1;

        BinTreeNode *new_node = btree->root+btree->elements;
        new_node->data = data;
        new_node->idx_left = 0;
        new_node->idx_right = 0;

        return btree->elements;
    }

    return 0;
}

void
tree_binary_print(BinTree *btree) {

    /* Helper function to print the entire binary tree */
    void tree_binary_print_inorder(BinTree *btree, tree_idx idx) {
        if (idx == 0) return; 
        BinTreeNode* node = btree->root + idx;
        (void) tree_binary_print_inorder(btree, node->idx_left);
        (void) printf("%d ", node->data);
        (void) tree_binary_print_inorder(btree, node->idx_right);
    }

    BinTreeNode* root = btree->root;
    (void) printf("In-order traversal of the binary tree:\n");
    (void) printf("%d ", root->data);
    (void) tree_binary_print_inorder(btree, root->idx_left);
    (void) tree_binary_print_inorder(btree, root->idx_right);
    (void) printf("\n");
}

void
tree_binary_fill_randomly(BinTree *btree) {
    BinTreeNode *node_ptr = btree->root;

    int32_t *random_data = (int32_t*) malloc(sizeof(int32_t) * btree->capacity);
    assert(random_data);

    for (uint32_t i = 0; i < btree->capacity; i++) {
        random_data[i] = i;
    }

    for (uint32_t i = btree->capacity - 1; i > 0; i--) {
        int32_t temp = random_data[i];
        int32_t j = randint(0, i-1);
        random_data[i] = random_data[j];
        random_data[j] = temp;
    }

    (void) printf("btree->capacity = %d\n", btree->capacity);
    for (tree_idx idx = 0; idx < btree->capacity ; idx++) {
        node_ptr[idx].data = random_data[idx];
        node_ptr[idx].idx_left = (2*idx+1 < btree->capacity) ? 2*idx+1 : 0;
        node_ptr[idx].idx_right = (2*idx+2 < btree->capacity) ? 2*idx+2 : 0;
        btree->elements++;
    }

    free(random_data);
}

int
main() {
    (void) srand(time(NULL));
    BinTree btree = tree_binary_create(1000000, 16);
    /*BinTree btree = tree_binary_create(10, 16);*/
    (void) tree_binary_fill_randomly(&btree);
    (void) tree_binary_print(&btree);
    (void) tree_binary_destroy(btree);
    return 0;
}
