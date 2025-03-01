#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct Node {
    int key;
    struct Node* left;
    struct Node* right;
    int height;
};

int avl_get_height(struct Node* n) {
    if (n == NULL) return 0;
    return n->height;
}

/* Creates node */
struct Node* createNode(int key) {
    struct Node* node = (struct Node*)malloc(sizeof(struct Node));
    assert(node);
    node->key = key;
    node->left = NULL;
    node->right = NULL;
    node->height = 1; 
    return node;
}

/* Utility function to get the maximum of two integers */
static int max(int a, int b) {
    return (a > b) ? a : b;
}

// Function to get balance factor of a node
int getBalanceFactor(struct Node* n) {
    if (n == NULL) return 0;
    return avl_get_height(n->left) - avl_get_height(n->right);
}

struct Node* rightRotate(struct Node* y) {
    struct Node* x = y->left;
    struct Node* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(avl_get_height(y->left), avl_get_height(y->right)) + 1;
    x->height = max(avl_get_height(x->left), avl_get_height(x->right)) + 1;

    return x;
}

// Left rotation function
struct Node* leftRotate(struct Node* x)
{
    struct Node* y = x->right;
    struct Node* T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update heights
    x->height = max(avl_get_height(x->left), avl_get_height(x->right)) + 1;
    y->height = max(avl_get_height(y->left), avl_get_height(y->right)) + 1;

    return y;
}

// Function to insert a key into AVL tree
struct Node* insert(struct Node* node, int key)
{
    // 1. Perform standard BST insertion
    if (node == NULL)
        return createNode(key);

    if (key < node->key)
        node->left = insert(node->left, key);
    else if (key > node->key)
        node->right = insert(node->right, key);
    else // Equal keys are not allowed in BST
        return node;

    // 2. Update height of this ancestor node
    node->height = 1 + max(avl_get_height(node->left), avl_get_height(node->right));

    // 3. Get the balance factor of this ancestor node to check whether this node became unbalanced
    int balance = getBalanceFactor(node);

    // 4. If the node becomes unbalanced, then there are 4 cases

    // Left Left Case
    if (balance > 1 && key < node->left->key)
        return rightRotate(node);

    // Right Right Case
    if (balance < -1 && key > node->right->key)
        return leftRotate(node);

    // Left Right Case
    if (balance > 1 && key > node->left->key) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Right Left Case
    if (balance < -1 && key < node->right->key) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    // Return the (unchanged) node pointer
    return node;
}

// Function to perform inorder traversal of AVL tree
void inOrder(struct Node* root)
{
    if (root != NULL) {
        inOrder(root->left);
        printf("%d ", root->key);
        inOrder(root->right);
    }
}

// Function to free all nodes in the AVL tree
void avlDestroy(struct Node* root)
{
    if (root != NULL) {
        avlDestroy(root->left);
        avlDestroy(root->right);
        free(root);
    }
}

// Main function
int main() {
    struct Node* root = NULL;

    // Inserting nodes
    for (int i = 0; i < 1000000; i++) {
        root = insert(root, i);
    }

    // Print inorder traversal of the AVL tree
    printf("Inorder traversal of AVL tree: ");
    inOrder(root);
    printf("\n");

    // Destroy AVL tree to free allocated memory
    avlDestroy(root);
    return 0;
}
