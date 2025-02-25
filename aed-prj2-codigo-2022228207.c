/* Código feito para C99, compilado com GCC 14.2.1 e flags -O2 e --fast-math
 * Hardware Original: SATELLITE_C50-A PSCG6P-01YAR1, 
 * CPU: Intel i5-3320M (4) @ 3.300GHz, GPU: GPU: Intel 3rd Gen Core processor Graphics Controller
 * RAM: 7821MiB, SSD SATA3 1TB
 * 
 * Aluno: Vasco Alves, 2022228207
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#define TREE_BINARY_DARRAY_GROWTH 16
#define ROOT_IDX 0
#define LEFT 0
#define RIGHT 1
#define TREE_IDX_MAX 4294967295
#define SEED 95911405

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

extern BinTree  tree_binary_create(uint32_t initial_capacity, int32_t data); // Creates binary tree
extern void     tree_binary_destroy(BinTree btree); // Frees binary tree
extern int      tree_binary_resize(BinTree *btree); 
extern void     tree_binary_init(tree_idx node, int32_t data);
extern void     tree_binary_insert(BinTree btree, tree_idx where, tree_idx node, int dir);
extern tree_idx tree_binary_get_new_elem(BinTree *btree, int32_t data);
extern void     tree_binary_insert(BinTree btree, tree_idx where, tree_idx node, int dir);
extern void     tree_binary_print(BinTree *btree);
extern void     tree_binary_print_arr(BinTree *btree);
extern void     tree_binary_gen_conj_a(BinTree *btree);
extern void     tree_binary_gen_conj_b(BinTree *btree);
extern tree_idx tree_binary_search_key(BinTree *btree, int32_t key);


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
tree_binary_create(uint32_t initial_capacity, int32_t data) {
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
    (void) puts("\n");
}

void
tree_binary_print_arr(BinTree *btree) {

    BinTreeNode *root = btree->root;
    for (tree_idx idx = 0; idx < btree->elements; idx++) {
         (void) printf("root[%d] = %d ", idx, root[idx].data);
    }
    (void) puts("\n");
}

void
tree_binary_gen_conj_a(BinTree *btree) {
/* Conjunto em ordem crescente com repetição minima */ 
    BinTreeNode *node_ptr = btree->root;

    (void) printf("btree->capacity = %d\n", btree->capacity);
    for (tree_idx idx = 0; idx < btree->capacity ; idx++) {
        node_ptr[idx].data = idx;
        node_ptr[idx].idx_left = (2*idx+1 < btree->capacity) ? 2*idx+1 : 0;
        node_ptr[idx].idx_right = (2*idx+2 < btree->capacity) ? 2*idx+2 : 0;
        btree->elements++;
    }
}

void
tree_binary_gen_conj_b(BinTree *btree) {
/* Conjunto em ordem com repetição minima */ 
    BinTreeNode *node_ptr = btree->root;

    int32_t *random_data = (int32_t*) malloc(sizeof(int32_t) * btree->capacity);

    assert(random_data); 

    int offset = 0;
    for (uint32_t i = 0; i < btree->capacity; i++) {
        random_data[btree->capacity-1-i] = i - offset;
        if (randint(0,10) == 5) {
            offset += 1;
        }
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

void
tree_binary_gen_conj_c(BinTree *btree) {
/* Conjunto em ordem com repetição minima */ 
    BinTreeNode *node_ptr = btree->root;

    int32_t *random_data = (int32_t*) malloc(sizeof(int32_t) * btree->capacity);

    assert(random_data); 

    int offset = 0;
    for (uint32_t i = 0; i < btree->capacity; i++) {
        random_data[btree->capacity-1-i] = i - offset;
        if (randint(0,10) == 5) {
            offset += 1;
        }
    }

    // shjuffleadadadn

    (void) printf("btree->capacity = %d\n", btree->capacity);
    for (tree_idx idx = 0; idx < btree->capacity ; idx++) {
        node_ptr[idx].data = random_data[idx];
        node_ptr[idx].idx_left = (2*idx+1 < btree->capacity) ? 2*idx+1 : 0;
        node_ptr[idx].idx_right = (2*idx+2 < btree->capacity) ? 2*idx+2 : 0;
        btree->elements++;
    }

    free(random_data);
}

tree_idx
tree_binary_search_key(BinTree *btree, int32_t key) {

    /*Helper function */
    tree_idx tree_binary_search(BinTreeNode *root, tree_idx idx, int32_t key) {
        if (idx == TREE_IDX_MAX || idx == 0) return TREE_IDX_MAX; // Base case: invalid index

        BinTreeNode node = root[idx];
        if (node.data == key) return idx; // Key found

        // Search in the left subtree
        tree_idx left = tree_binary_search(root, node.idx_left, key);
        if (left != TREE_IDX_MAX) return left; // Key found in the left subtree

        // Search in the right subtree
        tree_idx right = tree_binary_search(root, node.idx_right, key);
        if (right != TREE_IDX_MAX) return right; // Key found in the right subtree

        return TREE_IDX_MAX; // Key not found
    }

    BinTreeNode* root = btree->root;
    if (root->data == key) return ROOT_IDX; 

    tree_idx left = tree_binary_search(root, root->idx_left, key);
    if (left != TREE_IDX_MAX) return left;

    tree_idx right = tree_binary_search(root, root->idx_right, key);
    if (right != TREE_IDX_MAX) return right;

    return TREE_IDX_MAX; 
}

#define AVERAGE 10000
#define TREESIZE 1000000

double tree_binary_test(BinTree btree);

int
main() {

    srand(SEED);

    BinTree btree = tree_binary_create(TREESIZE, 16);
    (void) tree_binary_gen_conj_a(&btree);
    (void) printf("Binary Tree (CONJ A) in %0.3lf ms.\n",  tree_binary_test(btree) );

    (void) tree_binary_gen_conj_b(&btree);
    (void) printf("Binary Tree (CONJ B) in %0.3lf ms.\n",  tree_binary_test(btree) );

    (void) tree_binary_destroy(btree);

    return 0;
}

double tree_binary_test(BinTree btree) {
    double time_total = 0;
    clock_t start = 0, end = 0;

    for (int i = 0; i < AVERAGE; i++) {
        int search = randint(0, TREESIZE-1);
        start = clock();
        (void) tree_binary_search_key(&btree, search);
        end = clock();
        /*if (result != TREESIZE) printf("Found %d in index %ld.\n", search, result);*/
        /*else printf("%d not found!.\n", search);*/
        time_total += ((double) (end - start)*1000) / CLOCKS_PER_SEC;
    }

    return time_total / AVERAGE;
}
