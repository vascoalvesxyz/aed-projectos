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

#define RESIZE_FACTOR 1.61803
#define IDX_INVALID 4294967295
#define SEED 95911405

#ifdef DEBUG
#define AVERAGE 100
#define TREESIZE 10000
#else
#define AVERAGE 10
#define TREESIZE 1000000
#endif 

typedef uint32_t idx_t;
typedef int32_t key_t;

typedef struct BinTreeNode {
    key_t data;         
    idx_t idx_left;  
    idx_t idx_right; 
} BinTreeNode; // TOTAL = 12 Bytes per node;

typedef struct BinaryTree {
    uint32_t capacity; 
    uint32_t elements;
    BinTreeNode *root; 
} BinTree ;

typedef struct AVLNode {
    idx_t left;
    idx_t right;
    int key;
    int height;
} AVLNode;


typedef struct AVLTree {
    AVLNode *nodes;
    idx_t tree_root;
    idx_t elements;
    idx_t capacity;
} AVLTree; // 20 bytes

/* === HELPER FUNCTIONS === */
static int      randint(int a, int b);
static int      max(int a, int b);
static key_t*   arr_gen_conj_a(key_t size); // ordem crescent, pouca repetição
static key_t*   arr_gen_conj_b(key_t size); // ordem decrescent, pouca repetição
static key_t*   arr_gen_conj_c(key_t size); // ordem aleatoria, pouca repetição
static key_t*   arr_gen_conj_d(key_t size); // ordem aleatoria, 90% repetidos
static void     arr_print(key_t* arr, key_t size);

/* ===== BINARY TREE ===== */
extern BinTree  tree_binary_create(uint32_t initial_capacity); // Creates binary tree with inicialized elements
extern void     tree_binary_destroy(BinTree btree); // Frees binary tree
extern void     tree_binary_resize(BinTree *btree);  // Resize binary tree
extern void     tree_binary_insert(BinTree *btree, key_t key); // insert key in binary tree, NO DUPLICATES
extern void     tree_binary_insert_arr(BinTree *btree, key_t* arr, key_t size); // insert array of keys
extern void     tree_binary_print_inorder(BinTree *btree); // in order print according to tree
extern void     tree_binary_print(BinTree *btree);  // print by levels for visual accuracy
extern idx_t    tree_binary_search_key_inorder(BinTree btree, int32_t key); // search for key in binary tree by order
extern idx_t    tree_binary_search_key_breadth(BinTree btree, int32_t key); // search for key in binary tree by breadth
extern double   tree_binary_test(key_t* arr);

/* ===== AVL TREE ===== */
extern AVLTree tree_avl_create(idx_t inicial_capacity);
extern void    tree_avl_destroy(AVLTree* avl);
extern void    tree_avl_resize(AVLTree *avl);
static int      _tree_avl_get_height(AVLTree* avl, idx_t index);
static int      _tree_avl_get_balance(AVLTree* avl, idx_t index);
static idx_t    _tree_avl_rotate_right(AVLTree *avl, idx_t y_index);
static idx_t    _tree_avl_rotate_left(AVLTree *avl, idx_t x_index);
static idx_t    _tree_avl_insert_recursive(AVLTree *avl, idx_t node_index, int key);
extern void     tree_avl_insert(AVLTree *avl, int key);
extern void     tree_avl_insert_arr(AVLTree *avl, key_t* arr, size_t size);
extern AVLNode* tree_avl_search(AVLTree *avl, int key);
extern void     tree_avl_in_order(AVLTree *avl); // in-order print

/* ==== FUNCTION DECLATRATIONS ==== */
static int 
randint(int a, int b) {
    if (a > b) {
        a ^= b;
        b ^= a ;
        a ^= b;
    }
    return a + rand() % (b - a + 1);
}

static int
max(int a, int b) {
    return (a > b) ? a : b; 
}

static key_t*
arr_gen_conj_a(key_t size) {
    key_t* new_arr = (key_t*) malloc( sizeof(key_t) * size);
    
    if (new_arr) {
        key_t offset = 0;
        new_arr[0] = 0; // não podes saltar um item atrás de 0
        for (key_t  i = 1; i < size; i++) {
            if (randint(0,9) == 0) offset += 1; 
            new_arr[i] = i - offset; 
        }
    }
    return new_arr;
} 

static key_t*
arr_gen_conj_b(key_t size) {
    key_t* new_arr = (key_t*) malloc( sizeof(key_t) * size);
    
    if (new_arr) {
        key_t offset = 0;
        new_arr[0] = 0; // não podes saltar um item atrás de 0
        for (key_t i = 1; i < size; i++) {
            if (randint(0,9) == 0) offset += 1; 
            new_arr[i] = size+1-i+offset; 
        }
    }

    return new_arr;
}

static key_t*
arr_gen_conj_c(key_t size) {
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

static key_t*
arr_gen_conj_d(key_t size) {
    key_t* new_arr = (key_t*) malloc( sizeof(key_t) * size);

    if (new_arr) {
        new_arr[0] = 0; // não podes saltar um item atrás de idx 0
        for (key_t i = 1; i < size; i++) {
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

static void
arr_print(key_t* arr, key_t size) {
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
    if (IDX_INVALID != tree_binary_search_key_breadth(*btree, key)) {
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
tree_binary_insert_arr(BinTree *btree, key_t* arr, key_t size) {
    for (key_t k = 0; k < size; k++ )
        tree_binary_insert(btree, arr[k]);
}

void
tree_binary_print_inorder(BinTree *btree) {

    /* Helper function to print the entire binary tree */
    void inorder(BinTree *btree, idx_t idx) {
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

idx_t
tree_binary_search_key_inorder(BinTree btree, int32_t key) {

    /*Helper function */
    idx_t tree_binary_search(BinTreeNode *root, idx_t idx, int32_t key) {
        if (idx == IDX_INVALID || idx == 0) return IDX_INVALID; 

        BinTreeNode node = root[idx];
        if (node.data == key) return idx; 

        idx_t left = tree_binary_search(root, node.idx_left, key);
        if (left != IDX_INVALID) return left; 

        idx_t right = tree_binary_search(root, node.idx_right, key);
        if (right != IDX_INVALID) return right; 

        return IDX_INVALID; 
    }

    BinTreeNode* root = btree.root;
    if (root->data == key) return 0; 

    idx_t left = tree_binary_search(root, root->idx_left, key);
    if (left != IDX_INVALID) return left;

    idx_t right = tree_binary_search(root, root->idx_right, key);
    if (right != IDX_INVALID) return right;
    

    return IDX_INVALID; 
}

idx_t
tree_binary_search_key_breadth(BinTree btree, int32_t key) {
    BinTreeNode *root = btree.root;
    for (idx_t index = 0; index < btree.elements; index++) {
        if ( root[index].data == key )
            return index;
    }
    return IDX_INVALID;
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
    if (new_capacity >= IDX_INVALID) {
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
_tree_avl_get_height(AVLTree* avl, idx_t index) {
    return (index == IDX_INVALID) ? 0 : avl->nodes[index].height;
}

static int
_tree_avl_get_balance(AVLTree* avl, idx_t index) {
    if (index == IDX_INVALID) return 0;
    return _tree_avl_get_height(avl, avl->nodes[index].left) - _tree_avl_get_height(avl, avl->nodes[index].right);
}

static uint32_t g_rotation_count = 0;

static idx_t
_tree_avl_rotate_right(AVLTree *avl, idx_t y_index) {
    g_rotation_count++;

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
    g_rotation_count++;

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
        if (avl->nodes[current_index].key == key)
            return &avl->nodes[current_index];
            
        else if (key < avl->nodes[current_index].key)
            current_index = avl->nodes[current_index].left;

        else
            current_index = avl->nodes[current_index].right;
    }
    return NULL;  // Key not found.
}


void
tree_avl_insert_arr(AVLTree *avl, key_t* arr, size_t size) {
    assert(avl && arr);
    for (key_t* endptr = arr+size; arr != endptr && arr != NULL; arr++) {
        tree_avl_insert(avl, *arr);
    }
}


double
tree_avl_test(key_t* arr) {

    AVLTree avl;
    clock_t start = 0, end = 0;
    clock_t total = 0;

    /* Reset global rotation counter */
    g_rotation_count = 0;

    for (int i = 0; i < AVERAGE; i++) {
        start = clock();
        avl = tree_avl_create(10);
        tree_avl_insert_arr(&avl, arr, TREESIZE);
        end = clock();

        total += (end-start);
        tree_avl_destroy(&avl);
    }

    printf("=> Average Number of Rotations: %d\n", g_rotation_count/AVERAGE);

    double total_time = ((double) total*1000) / CLOCKS_PER_SEC;
    return total_time / AVERAGE;
}

int
main() {

    srand(SEED);
    key_t *key_ptr;

    /*key_ptr = arr_gen_conj_a(TREESIZE);*/
    /*printf("Binary Tree - Conj. A = %0.4lfms\n", tree_binary_test(key_ptr));*/
    /*free(key_ptr);*/
    /**/
    /*key_ptr = arr_gen_conj_b(TREESIZE);*/
    /*printf("Binary Tree - Conj. B = %0.4lfms\n", tree_binary_test(key_ptr));*/
    /*free(key_ptr);*/
    /**/
    /*key_ptr = arr_gen_conj_c(TREESIZE);*/
    /*printf("Binary Tree - Conj. C = %0.4lfms\n", tree_binary_test(key_ptr));*/
    /*free(key_ptr);*/
    /**/
    /*key_ptr = arr_gen_conj_d(TREESIZE);*/
    /*printf("Binary Tree - Conj. D = %0.4lfms\n", tree_binary_test(key_ptr));*/
    /*free(key_ptr);*/


    key_ptr = arr_gen_conj_a(TREESIZE);
    printf("AVL Tree - Conj. A = %0.4lfms\n", tree_avl_test(key_ptr));
    free(key_ptr);

    key_ptr = arr_gen_conj_b(TREESIZE);
    printf("AVL Tree - Conj. B = %0.4lfms\n", tree_avl_test(key_ptr));
    free(key_ptr);

    key_ptr = arr_gen_conj_c(TREESIZE);
    printf("AVL Tree - Conj. C = %0.4lfms\n", tree_avl_test(key_ptr));
    free(key_ptr);

    key_ptr = arr_gen_conj_d(TREESIZE);
    printf("AVL Tree - Conj. D = %0.4lfms\n", tree_avl_test(key_ptr));
    free(key_ptr);

    return 0;
}

