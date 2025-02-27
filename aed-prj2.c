/* Código feito para C99, compilado com GCC 14.2.1 com flags --std=c99 -O2 e --fast-math
 * Hardware Original: SATELLITE_C50-A PSCG6P-01YAR1, 
 * CPU: Intel i5-3320M (4) @ 3.300GHz, GPU: Intel 3rd Gen Core processor Graphics Controller
 * RAM: 7821MiB, SSD SATA3 1TB
 * 
 * Aluno: Vasco Alves, 2022228207
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#define MAX(a,b) ( (a > b) ? a : b )

#define RESIZE_FACTOR 1.61803
#define TREE_IDX_MAX 4294967295
#define SEED 95911405

#define _DEBUG_
#ifdef _DEBUG_
#define AVERAGE 1
#define TREESIZE 10000
#else
#define AVERAGE 10
#define TREESIZE 1000000
#endif 

/*static const unsigned int */

typedef uint32_t tree_idx;
typedef int32_t key_t;

typedef struct BinTreeNode {
    key_t data;         // 4 bytes
    tree_idx idx_left;  // 4 bytes
    tree_idx idx_right; // 4 bytes
} BinTreeNode;          // TOTAL = 12 Bytes;

typedef struct BinaryTree {
    uint32_t capacity; // 4 bytes
    uint32_t elements; // 4 bytes
    BinTreeNode *root; // 8 byres
} BinTree ;


typedef struct AVLTreeNode {
    struct AVLTreeNode *left;   // 8 bytes
    struct AVLTreeNode *right;  // 8 bytes
    uint32_t height;            // 4 bytes
    key_t key;                  // 4 bytes
} AVLTreeNode;                  // TOTAL = 24 bytes

typedef struct AVLTree {
    uint32_t capacity; // 4 bytes
    uint32_t elements; // 4 bytes
    struct AVLTreeNode *root; // 8 byres
} AVLTree;

/* Helper function */
extern int randint(int a, int b);
extern key_t*   arr_gen_conj_a(uint64_t size); // ordem crescent, pouca repetição
extern key_t*   arr_gen_conj_b(uint64_t size); // ordem decrescent, pouca repetição
extern key_t*   arr_gen_conj_c(uint64_t size); // ordem aleatoria, pouca repetição
extern key_t*   arr_gen_conj_d(uint64_t size); // ordem aleatoria, 90% repetidos
extern void     arr_print(key_t* arr, uint64_t size);

/* Binary Tree */
extern BinTree  tree_binary_create(uint32_t initial_capacity); // Creates binary tree with inicialized elements
extern void     tree_binary_destroy(BinTree btree); // Frees binary tree
extern void     tree_binary_resize(BinTree *btree);  // Resize binary tree
extern void     tree_binary_insert(BinTree *btree, key_t key); // insert key in binary tree, NO DUPLICATES
extern void     tree_binary_insert_arr(BinTree *btree, key_t* arr, size_t size); // insert array of keys

/* Printin and testing the binary tree */
extern void     tree_binary_print_inorder(BinTree *btree); // in order print according to tree
extern void     tree_binary_print(BinTree *btree);  // print by levels for visual accuracy
extern tree_idx tree_binary_search_key_inorder(BinTree btree, int32_t key); // search for key in binary tree
extern tree_idx tree_binary_search_key_breadth(BinTree btree, int32_t key); // search for key in binary tree
extern double   tree_binary_test(key_t* arr);

/* AVL Tree */
extern AVLTree  tree_avl_create(uint32_t initial_capacity); 
extern void     tree_avl_destroy(AVLTree avl); 
extern void     tree_avl_resize(AVLTree *avl);  
extern tree_idx tree_avl_insert(AVLTree *avl, tree_idx root_idx, key_t key);
extern void     tree_avl_insert_arr(AVLTree *avl, key_t* arr, size_t size);

int 
randint(int a, int b) {
    if (a > b) {
        a ^= b;
        b ^= a ;
        a ^= b;
    }
    return a + rand() % (b - a + 1);
}

key_t*
arr_gen_conj_a(uint64_t size) {
    key_t* new_arr = (key_t*) malloc( sizeof(key_t) * size);
    
    if (new_arr) {
        key_t offset = 0;
        new_arr[0] = 0; // não podes saltar um item atrás de 0
        for (uint64_t i = 1; i < size; i++) {
            if (randint(0,9) == 0) offset += 1; 
            new_arr[i] = i - offset; 
        }
    }
    return new_arr;
} 

key_t*
arr_gen_conj_b(uint64_t size) {
    key_t* new_arr = (key_t*) malloc( sizeof(key_t) * size);
    
    if (new_arr) {
        key_t offset = 0;
        new_arr[0] = 0; // não podes saltar um item atrás de 0
        for (uint64_t i = 1; i < size; i++) {
            if (randint(0,9) == 0) offset += 1; 
            new_arr[i] = size+1-i+offset; 
        }
    }

    return new_arr;
}

key_t*
arr_gen_conj_c(uint64_t size) {
    /* Array crescente com repetição minima */
    key_t* new_arr = arr_gen_conj_a(size);
    
    if (new_arr) {
        /* Knuth Shuffle */
        int i, j; 
        for (j = size-1; j > 0; j--) {
            i = randint(0, j-1);
            new_arr[i] ^= new_arr[j];
            new_arr[j] ^= new_arr[i];
            new_arr[i] ^= new_arr[j];
        }
    }
    return new_arr;
} 

key_t*
arr_gen_conj_d(uint64_t size) {
    key_t* new_arr = (key_t*) malloc( sizeof(key_t) * size);

    if (new_arr) {
        new_arr[0] = 0; // não podes saltar um item atrás de idx 0
        for (uint64_t i = 1; i < size; i++) {
                new_arr[i] = (randint(0,9) == 3) ? i : new_arr[i-1]; 
        }

        /* Knuth Shuffle */
        int i, j; 
        for (j = size-1; j > 0; j--) {
            i = randint(0, j-1);
            new_arr[i] ^= new_arr[j];
            new_arr[j] ^= new_arr[i];
            new_arr[i] ^= new_arr[j];
        }

    }

    return new_arr;
} 



void
arr_print(key_t* arr, uint64_t size) {
    for (key_t k = 0; k < size; k++)
        printf("arr[%d] = %d\n", k, arr[k]);
}


BinTree
tree_binary_create(uint32_t initial_capacity) {
    BinTree btree = {initial_capacity, 0, NULL};
    btree.root = (BinTreeNode*) malloc(sizeof(BinTreeNode)*initial_capacity);

    if (btree.root) {
        BinTreeNode* nodeptr = btree.root;
        for (BinTreeNode* endptr = nodeptr + initial_capacity; nodeptr != endptr; nodeptr++ ) {
            nodeptr->data = 0;
            nodeptr->idx_left = 0;
            nodeptr->idx_right = 0;
        }
    }

    return btree;
}

void
tree_binary_destroy(BinTree btree) {
    free(btree.root);
}

void
tree_binary_resize(BinTree *btree) {
    uint32_t new_capacity = btree->capacity*RESIZE_FACTOR;
    /* In case of overflow */
    if (new_capacity < btree->capacity) return;

    BinTreeNode* new_root = NULL; 
    uint64_t tries = 0;
    while (new_root == NULL && tries < 1000) {
        new_root = (BinTreeNode*) realloc(btree->root, sizeof(BinTreeNode)*new_capacity);
        tries++;
    }

    if (new_root == NULL) {
        puts("Failed to allocate enough memory for tree resize.\n");
        exit(EXIT_FAILURE);
    }

    btree->root = new_root;
    btree->capacity = new_capacity;
}

void
tree_binary_insert(BinTree *btree, key_t key) {

    /* NA OPERAÇÃO DE INSERÇÃO QUANDO UMA CHAVE JÁ EXISTIR, NÃO É CRIADA NOVA CHAVE */
    if (TREE_IDX_MAX != tree_binary_search_key_breadth(*btree, key)) {
        return;
    };

    if (btree->elements == btree-> capacity) {
        /*puts("capacity exceeded");*/
        /*printf("capacity = %d\n", btree->capacity);*/
        tree_binary_resize(btree);
        /*printf("new capacity = %d\n", btree->capacity);*/
    }

    BinTreeNode* root = btree->root;
    BinTreeNode* node = NULL;
    uint32_t inicial_elements = btree->elements;

    /* Está vazia */
    if (inicial_elements == 0) {
        node = btree->root;
    } else {
        /* Devido às propriedades das àrvores binárias implicitas,
         * o indice corresponde ao número de elementos-1,
         * como é o próximo indice, -1+1 = 0 */
        node = &root[inicial_elements];
        /* O pai do NOVO filho está em (Nº Elementos+1) / 2 arredondado para baixo -1 para o indice
         * O que pode ser simplificado para simplesmente o numero de elements>>1 
         * O módulo diz-nos se é para a esquerda ou direita */
        BinTreeNode* parent_node = &root[inicial_elements>>1]; 
        if (inicial_elements % 2 == 0) {
            parent_node->idx_left = inicial_elements;
        } else {
            parent_node->idx_right = inicial_elements;
        }
    }


    /* Inserir nova chave  */
    node->data = key;
    btree->elements = inicial_elements + 1;
}

void
tree_binary_insert_arr(BinTree *btree, key_t* arr, size_t size) {
    for (key_t k = 0; k < size; k++ )
        tree_binary_insert(btree, arr[k]);
}

void
tree_binary_print_inorder(BinTree *btree) {

    /* Helper function to print the entire binary tree */
    void inorder(BinTree *btree, tree_idx idx) {
        if (idx == 0) return; 
        BinTreeNode* node = btree->root + idx;
        (void) inorder(btree, node->idx_left);
        (void) printf("%d ", node->data);
        (void) inorder(btree, node->idx_right);
    }

    BinTreeNode* root = btree->root;
    (void) printf("In-order traversal of the binary tree:\n");
    (void) printf("%d ", root->data);
    (void) inorder(btree, root->idx_left);
    (void) inorder(btree, root->idx_right);
    (void) puts("\n");
}

void
tree_binary_print(BinTree *btree) {

    uint32_t levels = 0;
    uint32_t n_elem = btree->elements;

    /* Contar niveis */
    while (n_elem > 1) {
        n_elem = n_elem >> 1;
        levels++;
    }

    BinTreeNode *root = btree->root;
    printf("%2d\n", root->data);

    /* Imprimir cada nivel */
    uint32_t idx = 1;
    uint32_t n_nodes = 1;
    for (uint32_t l = 0; l < levels; l++) {
        /* Cada nivel tem o dobro dos elementos no maximo  */
        n_nodes = n_nodes << 1;
        for (uint32_t i = 0; i < n_nodes; i++) {
            if (idx == btree->elements-1) break;
            printf("%2d  ", (root+idx)->data );
            idx++;
        }
        puts("");
    }

}

tree_idx
tree_binary_search_key_inorder(BinTree btree, int32_t key) {

    /*Helper function */
    tree_idx tree_binary_search(BinTreeNode *root, tree_idx idx, int32_t key) {
        if (idx == TREE_IDX_MAX || idx == 0) return TREE_IDX_MAX; 

        BinTreeNode node = root[idx];
        if (node.data == key) return idx; 

        tree_idx left = tree_binary_search(root, node.idx_left, key);
        if (left != TREE_IDX_MAX) return left; 

        // Search in the right subtree
        tree_idx right = tree_binary_search(root, node.idx_right, key);
        if (right != TREE_IDX_MAX) return right; 

        return TREE_IDX_MAX; 
    }

    BinTreeNode* root = btree.root;
    if (root->data == key) return 0; 

    tree_idx left = tree_binary_search(root, root->idx_left, key);
    if (left != TREE_IDX_MAX) return left;

    tree_idx right = tree_binary_search(root, root->idx_right, key);
    if (right != TREE_IDX_MAX) return right;
    

    return TREE_IDX_MAX; 
}

tree_idx
tree_binary_search_key_breadth(BinTree btree, int32_t key) {
    BinTreeNode *root = btree.root;
    for (tree_idx index = 0; index < btree.elements; index++) {
        if ( root[index].data == key )
            return index;
    }
    return TREE_IDX_MAX;
}

double
tree_binary_test(key_t* arr) {

    BinTree btree;
    clock_t start = 0, end = 0;
    clock_t total = 0;

    for (int i = 0; i < AVERAGE; i++) {
        printf("Iteration = %d\n", i);
        start = clock();
        btree = tree_binary_create(10);
        tree_binary_insert_arr(&btree, arr, TREESIZE);
        end = clock();

        total += (end-start);
        tree_binary_destroy(btree);
    }
    double total_time = ((double) total*1000) / CLOCKS_PER_SEC;
    return total_time / AVERAGE;
}

AVLTree
tree_avl_create(uint32_t initial_capacity) {
    AVLTree avltree = {initial_capacity, 0, NULL};
    avltree.root = (AVLTreeNode*) malloc(sizeof(AVLTreeNode)*initial_capacity);

    if (avltree.root) {
        AVLTreeNode* nodeptr = avltree.root;
        for (AVLTreeNode* endptr = nodeptr + initial_capacity; nodeptr != endptr; nodeptr++ ) {
            nodeptr->key = 0;
            nodeptr->left = NULL;
            nodeptr->right = NULL;
            nodeptr->height = 0;
        }
    }

    return avltree;
}

void
tree_avl_destroy(AVLTree avl) {
    free(avl.root);
}

void
tree_avl_resize(AVLTree *avl) {
    uint32_t new_capacity = avl->capacity*RESIZE_FACTOR;
    if (new_capacity <= avl->capacity) return; // overflow

    AVLTreeNode* new_root = NULL; 
    uint64_t tries = 0;
    while (new_root == NULL && tries < 1000) {
        new_root = (AVLTreeNode*) realloc(avl->root, sizeof(AVLTreeNode)*new_capacity);
        tries++;
    }

    if (new_root == NULL) {
        puts("Failed to allocate enough memory for tree resize.\n");
        exit(EXIT_FAILURE);
    }

    avl->root = new_root;
    avl->capacity = new_capacity;
}

static int32_t _avl_get_height(AVLTreeNode *node);
static int32_t _avl_get_balance(AVLTreeNode *node);
static AVLTreeNode* _avl_rotate_right(AVLTreeNode* node);
static AVLTreeNode* _avl_rotate_left(AVLTreeNode* node);

static int32_t 
_avl_get_height(AVLTreeNode *node) {
    return (node) ? (node->height) : 0;
}

static int32_t 
_avl_get_balance(AVLTreeNode *node) {
    return (node) ? (_avl_get_height(node->left) - _avl_get_height(node->right)) : 0;
}

static AVLTreeNode*
_avl_rotate_right(AVLTreeNode* node) {
    AVLTreeNode* pivot = node->left;
    AVLTreeNode* pivot_friend = pivot->right;

    pivot->right = node;
    node->left = pivot_friend;

    node->height = 1 + MAX(_avl_get_height(node->left), _avl_get_height(node->right));
    pivot->height = 1 + MAX(_avl_get_height(pivot->left), _avl_get_height(pivot->right));
    return pivot;
}

static AVLTreeNode*
_avl_rotate_left(AVLTreeNode* node) {
    AVLTreeNode* pivot = node->right;
    AVLTreeNode* pivot_friend = pivot->left;

    pivot->left = node;
    node->right = pivot_friend;

    node->height = 1 + MAX(_avl_get_height(node->left), _avl_get_height(node->right));
    pivot->height = 1 + MAX(_avl_get_height(pivot->left), _avl_get_height(pivot->right));
    return pivot;
}

void
tree_avl_insert(AVLTree* avl, key_t key) {

    AVLTreeNode *root = avl->root;
    printf("avl->elements = %d\n", avl->elements);

    /* Allocate more addresses if needed */
    if (avl->elements == avl->capacity)
        tree_avl_resize(avl);

    AVLTreeNode stack[avl->capacity];

    /* Get next node adress */
    AVLTreeNode* new_node = root+avl->elements;

    /* First node is already in it's place */
    if (avl->elements == 0) {
        new_node->key = key;
        new_node->left = new_node->right = NULL;
        new_node->height = 1;
        avl->elements++;
        return;
    }

    /* Find parent */
    AVLTreeNode* parent = NULL;
    AVLTreeNode* current = root;
    while (current != NULL) {
        parent = current;
        if (key < current->key)
            current = current->left;
        else if (key > current->key)
            current = current->right;
        else return; 
    }

    /* Initialize new node */
    new_node->key = key;
    new_node->left = new_node->right = NULL;
    new_node->height = 1;
    avl->elements++;

    /* Attach new node to correct parent */
    if (key < parent->key) {
        parent->left = new_node;
    } else {
        parent->right = new_node;
    }


}

void
tree_avl_insert_arr(AVLTree *avl, key_t* arr, size_t size) {
    assert(avl && arr);
    for (key_t* endptr = arr+size; arr != endptr && arr != NULL; arr++) {
        tree_avl_insert(avl, *arr);
    }
}

void
tree_avl_print_inorder(AVLTreeNode* root) {
    if (root) {
        tree_avl_print_inorder(root->left);
        printf("%d ", root->key);
        tree_avl_print_inorder(root->right);
    }
}

int
main() {

    srand(SEED);

    key_t *key_ptr = arr_gen_conj_a(TREESIZE);
    double testtime = tree_binary_test(key_ptr);
    printf("Binary Tree - Conj. A = %lfms\n", testtime);
    free(key_ptr);

    key_ptr = arr_gen_conj_b(TREESIZE);
    testtime = tree_binary_test(key_ptr);
    printf("Binary Tree - Conj. B = %lfms\n", testtime);
    free(key_ptr);

    key_ptr = arr_gen_conj_c(TREESIZE);
    testtime = tree_binary_test(key_ptr);
    printf("Binary Tree - Conj. C = %lfms\n", testtime);
    free(key_ptr);

    key_ptr = arr_gen_conj_d(TREESIZE);
    /*arr_print(key_ptr, TREESIZE);*/
    testtime = tree_binary_test(key_ptr);
    printf("Binary Tree - Conj. D = %lfms\n", testtime);
    free(key_ptr);


    AVLTree avl = tree_avl_create(10);
    tree_avl_insert_arr(avl, , TREESIZE);

    tree_avl_print_inorder(avl.root);
    for (int i = 0; i < 10; i++) 
        printf("avl.root[%d] = %d\n", i, avl.root[i].key);

    tree_avl_destroy(avl);
    return 0;
}

