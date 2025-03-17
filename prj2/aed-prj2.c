/* Código feito para C99, compilado com GCC 14.2.1 com flags --std=c99 -O2 e --fast-math
 *
 * Hardware Original: TOSHIBA SATELLITE_C50-A PSCG6P-01YAR1, 
 * CPU: Intel i5-3320M (4) @ 3.300GHz,
 * GPU: Intel 3rd Gen Core processor Graphics Controller
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

#define BLACK 0
#define RED 1

#define DEBUG
#ifdef DEBUG
#define AVERAGE 10
#define TREESIZE 10000
#else
#define AVERAGE 5
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
    idx_t tree_root; // rotations cause the root to change
    idx_t elements;
    idx_t capacity;
} AVLTree; // 20 bytes

typedef struct RBNode {
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

/* === HELPER FUNCTIONS === */
static inline int randint(int a, int b);
static inline int max(int a, int b);
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
extern idx_t    tree_binary_search_key_level(BinTree btree, int32_t key); // faster than inorder because of this structure
extern void     binary_test_and_log(key_t* arr, FILE *fptr);

/* ===== AVL TREE ===== */
extern AVLTree tree_avl_create(idx_t inicial_capacity);
extern void    tree_avl_destroy(AVLTree* avl);
extern void    tree_avl_resize(AVLTree *avl);
static int      _avl_get_height(AVLTree* avl, idx_t index);
static int      _avl_get_balance(AVLTree* avl, idx_t index);
static idx_t    _avl_rotate_right(AVLTree *avl, idx_t y_index);
static idx_t    _avl_rotate_left(AVLTree *avl, idx_t x_index);
static idx_t    _avl_insert_recursive(AVLTree *avl, idx_t node_index, int key);
extern void     tree_avl_insert(AVLTree *avl, int key);
extern void     tree_avl_insert_arr(AVLTree *avl, key_t* arr, size_t size);
extern AVLNode* tree_avl_search(AVLTree *avl, int key);
extern void     tree_avl_in_order(AVLTree *avl); // in-order print

/* ===== RED BLACK TREE ===== */
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

/* ===== TREAP ===== */ 
extern Treap tree_treap_create(int inicial_capacity);
extern void  tree_treap_resize(Treap *treap);
extern void  tree_treap_destroy(Treap *treap);
static idx_t _treap_rotate_right(Treap *treap, idx_t x_idx);
static idx_t _treap_rotate_left(Treap *treap, idx_t x_idx);
static idx_t _treap_insert_recursive(Treap *treap, idx_t idx, key_t key);
extern void  tree_treap_insert(Treap *treap, key_t key);

/* ==== FUNCTION DECLATRATIONS ==== */
static inline int 
randint(int a, int b) {
    if (a > b) {
        a ^= b;
        b ^= a ;
        a ^= b;
    }
    return a + rand() % (b - a + 1);
}

static inline int
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
    if (IDX_INVALID != tree_binary_search_key_level(*btree, key)) {
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
tree_binary_search_key_level(BinTree btree, int32_t key) {

    /* Como não existe ordem inerente nesta àrvore binária, os nós estão
     * inseridos no array da esquerda para a direita, logo posso percorrer o array.
     * Vou optimizar porque sim. */

    register BinTreeNode *ptr_front = btree.root;
    register BinTreeNode *ptr_back = btree.root + btree.elements;

    /*printf("Search key = %d\n", key);*/

    register int found = 0;
    while (ptr_front + 7 < ptr_back) {  

        /* prefetch */
        __builtin_prefetch(ptr_front + 32, 0, 1);
        __builtin_prefetch(ptr_back - 32, 0, 1);

        /* Unrolling */
        found |= (ptr_front->data == key);
        found |= ((ptr_front + 1)->data == key);
        found |= ((ptr_front + 2)->data == key);
        found |= ((ptr_front + 3)->data == key);
        
        found |= (ptr_back->data == key);
        found |= ((ptr_back - 1)->data == key);
        found |= ((ptr_back - 2)->data == key);
        found |= ((ptr_back - 3)->data == key);

        ptr_front += 4;
        ptr_back -= 4;

        if (found) return 0;
    }

    /* elementos restante */
    while (ptr_front <= ptr_back) {
        found |= (ptr_front->data == key) | (ptr_back->data == key);
        ptr_front++;
        ptr_back--;
    }

    return found ? 0 : IDX_INVALID;
}

void
binary_test_and_log(key_t* arr, FILE *fptr) {

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
    fprintf(fptr, "Binary Tree = %0.4lfms\t(0 rotations)\n", total_time/AVERAGE);
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
_avl_get_height(AVLTree* avl, idx_t index) {
    return (index == IDX_INVALID) ? 0 : avl->nodes[index].height;
}

static int
_avl_get_balance(AVLTree* avl, idx_t index) {
    if (index == IDX_INVALID) return 0;
    return _avl_get_height(avl, avl->nodes[index].left) - _avl_get_height(avl, avl->nodes[index].right);
}

static uint32_t g_rotation_count = 0;

static idx_t
_avl_rotate_right(AVLTree *avl, idx_t node_idx) {
    g_rotation_count++;

    idx_t pivot = avl->nodes[node_idx].left;
    idx_t T2 = avl->nodes[pivot].right;

    // Perform rotation:
    avl->nodes[pivot].right = node_idx;
    avl->nodes[node_idx].left = T2;

    // Update heights:
    avl->nodes[node_idx].height = 1 + max(_avl_get_height(avl, avl->nodes[node_idx].left), _avl_get_height(avl, avl->nodes[node_idx].right));
    avl->nodes[pivot].height = 1 + max(_avl_get_height(avl, avl->nodes[pivot].left), _avl_get_height(avl, avl->nodes[pivot].right));
    return pivot;
}

static idx_t
_avl_rotate_left(AVLTree *avl, idx_t x_index) {
    g_rotation_count++;

    idx_t pivot = avl->nodes[x_index].right;
    idx_t T2 = avl->nodes[pivot].left;

    // Perform rotation:
    avl->nodes[pivot].left = x_index;
    avl->nodes[x_index].right = T2;

    // Update heights:
    avl->nodes[x_index].height = 1 + max(_avl_get_height(avl, avl->nodes[x_index].left), _avl_get_height(avl, avl->nodes[x_index].right));
    avl->nodes[pivot].height = 1 + max(_avl_get_height(avl, avl->nodes[pivot].left), _avl_get_height(avl, avl->nodes[pivot].right));
    return pivot;
}

static idx_t
_avl_insert_recursive(AVLTree *avl, idx_t node_index, int key) {
    
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
        avl->nodes[node_index].left = _avl_insert_recursive(avl, avl->nodes[node_index].left, key);
    } else if (key > avl->nodes[node_index].key) {
        avl->nodes[node_index].right = _avl_insert_recursive(avl, avl->nodes[node_index].right, key);
    } else {
        return node_index;
    }
    
    avl->nodes[node_index].height = 1 + max(_avl_get_height(avl, avl->nodes[node_index].left),
                                             _avl_get_height(avl, avl->nodes[node_index].right));

    // Get balance factor to check if rebalancing is needed.
    int balance = _avl_get_balance(avl, node_index);

    /* Left Left */
    if (balance > 1 && key < avl->nodes[avl->nodes[node_index].left].key)
        return _avl_rotate_right(avl, node_index);

    /* Right Right */
    if (balance < -1 && key > avl->nodes[avl->nodes[node_index].right].key)
        return _avl_rotate_left(avl, node_index);

    /* Left Right */
    if (balance > 1 && key > avl->nodes[avl->nodes[node_index].left].key) {
        avl->nodes[node_index].left = _avl_rotate_left(avl, avl->nodes[node_index].left);
        return _avl_rotate_right(avl, node_index);
    }

    /* Right Left */
    if (balance < -1 && key < avl->nodes[avl->nodes[node_index].right].key) {
        avl->nodes[node_index].right = _avl_rotate_right(avl, avl->nodes[node_index].right);
        return _avl_rotate_left(avl, node_index);
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
        avl->tree_root = _avl_insert_recursive(avl, IDX_INVALID, key);
    } else {
        avl->tree_root = _avl_insert_recursive(avl, avl->tree_root, key);
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


void
avl_test_and_log(key_t* arr, FILE *fptr) {

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

    double total_time = ((double) total*1000) / CLOCKS_PER_SEC;
    fprintf(fptr, "AVL Tree = %0.4lfms\t(%d rotations)\n", total_time/AVERAGE, g_rotation_count/AVERAGE);
}

/* Red Black Tree Implementation */
/* Criar arvore */
RBTree
tree_rb_create(uint32_t initial_capacity) {
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
void
tree_rb_destroy(RBTree *rb) {
    free(rb->nodes);
}

/* Aumentar capacidade */
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


/* 1 (verdadeiro) se o nó for vermelho */
static int
_rb_is_red(RBTree *tree, idx_t i) {
    // a raiz da arvore é invalida inicialmente
    // isto significa que vai ser pintada correctamente de preto
    if (i == IDX_INVALID) return 0; 
    return (tree->nodes[i].color == RED);
}

/* rotação à esquerda */
static idx_t
_rb_rotate_left(RBTree *tree, idx_t h) {
    g_rotation_count++;
    idx_t pivot = tree->nodes[h].right;
    tree->nodes[h].right = tree->nodes[pivot].left;
    tree->nodes[pivot].left = h;
    tree->nodes[pivot].color = tree->nodes[h].color;
    tree->nodes[h].color = RED;
    return pivot;
}

/* rotação à direita */
static idx_t
_rb_rotate_right(RBTree *tree, idx_t h) {
    g_rotation_count++;
    idx_t pivot = tree->nodes[h].left;
    tree->nodes[h].left = tree->nodes[pivot].right;
    tree->nodes[pivot].right = h;
    tree->nodes[pivot].color = tree->nodes[h].color;
    tree->nodes[h].color = RED;
    return pivot;
}

/* Inverter cores */
static void
_rb_flip_colors(RBTree *tree, idx_t h) {
    tree->nodes[h].color = !tree->nodes[h].color;
    idx_t left = tree->nodes[h].left;
    idx_t right = tree->nodes[h].right;
    if (left != IDX_INVALID)
        tree->nodes[left].color = !tree->nodes[left].color;
    if (right != IDX_INVALID)
        tree->nodes[right].color = !tree->nodes[right].color;
}

/* Resolve comflictos */
static idx_t
_rb_fix_up(RBTree *tree, idx_t h) {
    /* Caso 1: direita vermelha e esquerda preta -> rotação à esquerda */
    if (_rb_is_red(tree, tree->nodes[h].right) && !_rb_is_red(tree, tree->nodes[h].left))
        h = _rb_rotate_left(tree, h);
    /* Caso 2: filho esquerdo vermelho e neto esquerdo vermelho -> rotação à direita */
    if (_rb_is_red(tree, tree->nodes[h].left) && _rb_is_red(tree, tree->nodes[tree->nodes[h].left].left))
        h = _rb_rotate_right(tree, h);
    /* Caso 3: ambos os filhos são vermelhos */
    if (_rb_is_red(tree, tree->nodes[h].left) && _rb_is_red(tree, tree->nodes[h].right))
        _rb_flip_colors(tree, h);
    return h;
}

/* Inserção recursiva: Devolve o novo indice da raiz se inserir */
static idx_t
_rb_insert_recursive(RBTree *tree, idx_t h, key_t key) {

    /* Inserir após encontrar nova folha 
     * (chamada anterior para filho que não existe) */
    if (h == IDX_INVALID) {
        idx_t new_index = tree->elements;
        tree->nodes[new_index].key = key;
        tree->nodes[new_index].left = IDX_INVALID;
        tree->nodes[new_index].right = IDX_INVALID;
        tree->nodes[new_index].color = RED;  // sempre vermelho
        tree->elements++;
        return new_index;
    }
    
    /* Recursão equivalente a binary search tree */
    if (key < tree->nodes[h].key) {
        tree->nodes[h].left = _rb_insert_recursive(tree, tree->nodes[h].left, key);
    } else if (key > tree->nodes[h].key) {
        tree->nodes[h].right = _rb_insert_recursive(tree, tree->nodes[h].right, key);
    }

    /* É necessário corrigir erros causados pela inserção */
    h = _rb_fix_up(tree, h);
    return h;
}

/* Inserir nó */
void
tree_rb_insert(RBTree *tree, key_t key) {

    /* Aumentar capacidade  se for necessário */
    if (tree->capacity == tree->elements)
        tree_rb_resize(tree);

    tree->tree_root = _rb_insert_recursive(tree, tree->tree_root, key);
    tree->nodes[tree->tree_root].color = BLACK;
}

/* Pesquisa */
int
tree_rb_search(RBTree *tree, int key) {
    RBNode *nodes = tree->nodes;
    idx_t current = tree->tree_root;

    /* binary search tree search */
    while (current != IDX_INVALID) {
        printf("current = %d\n", current);
        if (key < nodes[current].key)
            current = nodes[current].left;
        else if (key > nodes[current].key)
            current = nodes[current].right;
        else
            return current;
    }

    return -1;
}


void
rb_test_and_log(key_t* arr, FILE *fptr) {

    RBTree vp;
    clock_t start = 0, end = 0;
    clock_t total = 0;

    /* Reset global rotation counter */
    g_rotation_count = 0;

    for (int i = 0; i < AVERAGE; i++) { start = clock();
        vp = tree_rb_create(TREESIZE);
        for (idx_t idx = 0; idx < TREESIZE; idx++)
            tree_rb_insert(&vp, arr[idx]);
        end = clock();

        total += (end-start);
        tree_rb_destroy(&vp);
    }

    double total_time = ((double) total*1000) / CLOCKS_PER_SEC;
    fprintf(fptr, "RB Tree = %0.4lfms\t(%d rotations)\n", total_time/AVERAGE, g_rotation_count/AVERAGE);
}

/* Treap Functions */
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

static idx_t
_treap_rotate_right(Treap *treap, idx_t y_idx) {
    g_rotation_count++;
    TreapNode *nodes = treap->nodes;
    TreapNode *y = &nodes[y_idx];
    idx_t x_idx = y->left;
    TreapNode *x = &nodes[x_idx];
    
    y->left = x->right;
    x->right = y_idx;
    
    return x_idx;
}

static idx_t
_treap_rotate_left(Treap *treap, idx_t x_idx) {
    g_rotation_count++;
    TreapNode *nodes = treap->nodes;
    TreapNode *x = &nodes[x_idx];
    idx_t y_idx = x->right;
    TreapNode *y = &nodes[y_idx];
    
    x->right = y->left;
    y->left = x_idx;
    
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

void
treap_test_and_log(key_t* arr, FILE *fptr) {

    Treap treap;
    clock_t start = 0, end = 0;
    clock_t total = 0;

    /* Reset global rotation counter */
    g_rotation_count = 0;

    for (int i = 0; i < AVERAGE; i++) {
        start = clock();
        treap = tree_treap_create(TREESIZE);
        for (idx_t idx = 0; idx < TREESIZE; idx++)
            tree_treap_insert(&treap, arr[idx]);
        end = clock();

        total += (end-start);
        tree_treap_destroy(&treap);
    }

    double total_time = ((double) total*1000) / CLOCKS_PER_SEC;
    fprintf(fptr, "RB Tree = %0.4lfms\t(%d rotations)\n", total_time/AVERAGE, g_rotation_count/AVERAGE);
}

int
main() {

    srand(SEED);
    key_t *conjunto_a = arr_gen_conj_a(TREESIZE);
    key_t *conjunto_b = arr_gen_conj_b(TREESIZE);
    key_t *conjunto_c = arr_gen_conj_c(TREESIZE);
    key_t *conjunto_d = arr_gen_conj_d(TREESIZE);

    FILE* filelog = fopen("log.txt", "a");
    fprintf(filelog, "\n=== NEW LOG ===\n");

    binary_test_and_log(conjunto_a, filelog);
    binary_test_and_log(conjunto_b, filelog);
    binary_test_and_log(conjunto_c, filelog);
    binary_test_and_log(conjunto_d, filelog);

    avl_test_and_log(conjunto_a, filelog);
    avl_test_and_log(conjunto_b, filelog);
    avl_test_and_log(conjunto_c, filelog);
    avl_test_and_log(conjunto_d, filelog);

    rb_test_and_log(conjunto_a, filelog);
    rb_test_and_log(conjunto_b, filelog);
    rb_test_and_log(conjunto_c, filelog);
    rb_test_and_log(conjunto_d, filelog);

    treap_test_and_log(conjunto_a, filelog);

    free(conjunto_a);
    free(conjunto_b);
    free(conjunto_c);
    free(conjunto_d);
    fclose(filelog);

    puts("Done!");
    return 0;
}

