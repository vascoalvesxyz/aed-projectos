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


/* 1 (verdadeiro) se o nó for vermelho */
static int _rb_is_red(RBTree *tree, idx_t i) {
    // a raiz da arvore é invalida inicialmente
    // isto significa que vai ser pintada correctamente de preto
    if (i == IDX_INVALID) return 0; 
    return (tree->nodes[i].color == RED);
}

/* rotação à esquerda */
static idx_t _rb_rotate_left(RBTree *tree, idx_t h) {
    idx_t pivot = tree->nodes[h].right;
    tree->nodes[h].right = tree->nodes[pivot].left;
    tree->nodes[pivot].left = h;
    tree->nodes[pivot].color = tree->nodes[h].color;
    tree->nodes[h].color = RED;
    return pivot;
}

/* rotação à direita */
static idx_t _rb_rotate_right(RBTree *tree, idx_t h) {
    idx_t pivot = tree->nodes[h].left;
    tree->nodes[h].left = tree->nodes[pivot].right;
    tree->nodes[pivot].right = h;
    tree->nodes[pivot].color = tree->nodes[h].color;
    tree->nodes[h].color = RED;
    return pivot;
}

/* Inverter cores */
static void _rb_flip_colors(RBTree *tree, idx_t h) {
    tree->nodes[h].color = !tree->nodes[h].color;
    idx_t left = tree->nodes[h].left;
    idx_t right = tree->nodes[h].right;
    if (left != IDX_INVALID)
        tree->nodes[left].color = !tree->nodes[left].color;
    if (right != IDX_INVALID)
        tree->nodes[right].color = !tree->nodes[right].color;
}

/* Resolve comflictos */
static idx_t _rb_fix_up(RBTree *tree, idx_t h) {
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
static idx_t _rb_insert_recursive(RBTree *tree, idx_t h, key_t key) {

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
void tree_rb_insert(RBTree *tree, key_t key) {

    /* Aumentar capacidade  se for necessário */
    if (tree->capacity == tree->elements)
        tree_rb_resize(tree);

    tree->tree_root = _rb_insert_recursive(tree, tree->tree_root, key);
    tree->nodes[tree->tree_root].color = BLACK;
}

/* Pesquisa */
int tree_rb_search(RBTree *tree, int key) {
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

int
main(void) {

    RBTree tree = tree_rb_create(TREESIZE);

    int keys[] = {7, 9, 4, 11, 3, 5};
    size_t num_keys = sizeof(keys) / sizeof(keys[0]);

    for (size_t i = 0; i < num_keys; i++) {
        tree_rb_insert(&tree, keys[i]);
        printf("Inserted key %d\n", keys[i]);
    }

    printf("RB-tree built with %u nodes.\n", tree.elements);

     /*Search for some keys */
    int search_keys[] = {11, 5, 100};
    size_t num_search = sizeof(search_keys) / sizeof(search_keys[0]);
    for (size_t i = 0; i < num_search; i++) {
        int idx = tree_rb_search(&tree, search_keys[i]);
        if (idx != -1)
            printf("Found key %d at node index %d\n", search_keys[i], idx);
        else
            printf("Key %d not found in the tree.\n", search_keys[i]);
    }

    tree_rb_destroy(&tree);
    return 0;
}
