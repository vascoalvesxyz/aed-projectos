#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define IDX_INVALID 4294967295U

typedef uint32_t idx_t;
typedef int32_t key_t;

typedef struct Node {
    key_t key;
    idx_t priority;
    idx_t size;  // Subtree size
} Node;

typedef struct ImplicitTreap {
    Node *nodes;
    idx_t root;
    idx_t capacity;
    idx_t size;
} ImplicitTreap;

// Helper functions
idx_t get_size(ImplicitTreap *t, idx_t idx) {
    return (idx == IDX_INVALID) ? 0 : t->nodes[idx].size;
}

void update_size(ImplicitTreap *t, idx_t idx) {
    if (idx != IDX_INVALID) {
        t->nodes[idx].size = 1 + get_size(t, 2*idx+1) + get_size(t, 2*idx+2);
    }
}

void split(ImplicitTreap *t, idx_t idx, idx_t *left, idx_t *right, idx_t pos) {
    if (idx == IDX_INVALID) {
        *left = *right = IDX_INVALID;
        return;
    }
    
    idx_t curr_pos = get_size(t, 2*idx+1) + 1;
    
    if (curr_pos <= pos) {
        split(t, 2*idx+2, right, &t->nodes[idx].right, pos - curr_pos);
        *left = idx;
    } else {
        split(t, 2*idx+1, left, &t->nodes[idx].left, pos);
        *right = idx;
    }
    update_size(t, idx);
}

idx_t merge(ImplicitTreap *t, idx_t left, idx_t right) {
    if (left == IDX_INVALID) return right;
    if (right == IDX_INVALID) return left;
    
    if (t->nodes[left].priority > t->nodes[right].priority) {
        t->nodes[left].right = merge(t, 2*left+2, right);
        update_size(t, left);
        return left;
    } else {
        t->nodes[right].left = merge(t, left, 2*right+1);
        update_size(t, right);
        return right;
    }
}

// Public interface
void treap_insert(ImplicitTreap *t, idx_t pos, key_t key) {
    Node new_node = {
        .key = key,
        .priority = rand(),
        .size = 1
    };
    
    // Expand array if needed
    if (t->size == t->capacity) {
        t->capacity = t->capacity * 2 + 1;
        t->nodes = realloc(t->nodes, t->capacity * sizeof(Node));
    }
    
    idx_t new_idx = t->size++;
    t->nodes[new_idx] = new_node;
    
    idx_t left, right;
    split(t, t->root, &left, &right, pos);
    t->root = merge(t, merge(t, left, new_idx), right);
}

// Initialization
ImplicitTreap treap_create() {
    ImplicitTreap t = {
        .nodes = NULL,
        .root = IDX_INVALID,
        .capacity = 0,
        .size = 0
    };
    srand(time(NULL));
    return t;
}
