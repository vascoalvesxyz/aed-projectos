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
#define AVERAGE 10000
#define TREESIZE 1000

typedef uint32_t tree_idx;
typedef int32_t key_t;

typedef struct BinTreeNode {
    key_t data;
    tree_idx idx_left;
    tree_idx idx_right;
} BinTreeNode ;

typedef struct BinaryTree {
    uint32_t capacity;
    uint32_t elements;
    BinTreeNode *root;
} BinTree ;

/* Helper function */
extern int randint(int a, int b);
extern key_t*   arr_gen_conj_a(uint64_t size); // ordem crescent, pouca repetição
extern key_t*   arr_gen_conj_b(uint64_t size); // ordem decrescent, pouca repetição
extern key_t*   arr_gen_conj_c(uint64_t size); // ordem aleatoria, pouca repetição
extern key_t*   arr_gen_conj_c(uint64_t size); // ordem aleatoria, 90% repetidos
extern void     arr_print(key_t* arr, uint64_t size);

/* Binary Tree */
extern BinTree  tree_binary_create(uint32_t initial_capacity); // Creates binary tree with inicialized elements
extern void     tree_binary_destroy(BinTree btree); // Frees binary tree
extern void     tree_binary_resize(BinTree *btree);  // Resize binary tree
extern void     tree_binary_insert(BinTree *btree, key_t key); // insert key in binary tree
extern void     tree_binary_insert_arr(BinTree *btree, key_t* arr, size_t size); // insert array of keys

/* Printin and testing the binary tree */
extern void     tree_binary_print_inorder(BinTree *btree); // in order print according to tree
extern void     tree_binary_print(BinTree *btree); 
extern tree_idx tree_binary_search_key(BinTree btree, int32_t key); // search for key in binary tree
extern double   tree_binary_test(key_t* arr);

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
        for (uint64_t i = 0; i < size; i++) {
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
        for (uint64_t i = 0; i < size; i++) {
            if (randint(0,9) == 0) offset += 1; 
            new_arr[i] = size-i+offset; 
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
    uint16_t new_capacity = btree->capacity + TREE_BINARY_DARRAY_GROWTH;

    BinTreeNode* new_root = NULL; 
    uint64_t tries = 0;
    while (new_root == NULL && tries < 1000) {
        new_root = (BinTreeNode*) realloc(btree->root, sizeof(BinTreeNode)*new_capacity);
        tries++;
    }

    if (new_root == NULL) {
        puts("Failed to allocate enough memory for tree resize.");
        exit(EXIT_FAILURE);
    }

    btree->capacity = new_capacity;
    btree->root = new_root;
}

void
tree_binary_insert(BinTree *btree, key_t key) {
    if (btree->elements == btree-> capacity) {
        tree_binary_resize(btree);
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
        /* O módulo diz-nos se é para a esquerda ou direita */
        BinTreeNode* parent_node = &root[inicial_elements>>1]; 
        if (inicial_elements % 2 == 0) {
            parent_node->idx_left = inicial_elements;
        } else {
            parent_node->idx_right = inicial_elements;
        }
    }

    /* Inserir nova chave e  */
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
    /* Os niveis de um arevore binária completa implicita vão ter os indices
     * Nivel 1: 0
     * Nivel 2: 1, 2
     * Nivel 3: 3, 4, 5, 6
     * A nossa arvore não é perfeita por isso temos que verificar se cada indice tem pai
     */
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
tree_binary_search_key(BinTree btree, int32_t key) {

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

    BinTreeNode* root = btree.root;
    if (root->data == key) return ROOT_IDX; 

    tree_idx left = tree_binary_search(root, root->idx_left, key);
    if (left != TREE_IDX_MAX) return left;

    tree_idx right = tree_binary_search(root, root->idx_right, key);
    if (right != TREE_IDX_MAX) return right;

    return TREE_IDX_MAX; 
}

double
tree_binary_test(key_t* arr) {

    BinTree btree;
    clock_t start = 0, end = 0;
    clock_t total = 0;

    for (int i = 0; i < AVERAGE; i++) {
        start = clock();
        btree = tree_binary_create(TREESIZE);
        tree_binary_insert_arr(&btree, arr, TREESIZE);
        end = clock();

        total += (end-start);
        tree_binary_destroy(btree);
    }
    double total_time = ((double) total*1000) / CLOCKS_PER_SEC;
    return total_time / AVERAGE;
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

    return 0;
}
